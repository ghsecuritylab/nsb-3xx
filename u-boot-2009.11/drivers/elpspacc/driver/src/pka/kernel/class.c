/*
 * Copyright (c) 2013 Elliptic Technologies Inc.
 *
 * Device class for PKA-like devices.  This handles character device
 * allocation and the toplevel ioctl handling.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define CLASSNAME "pkadev"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "class.h"

static DEFINE_IDR(pka_idr);
static DEFINE_MUTEX(pka_idr_mutex);

static struct class *pka_class;
static int pka_major;

struct pka_chrdev_priv {
   struct device *pka_dev;
   const struct pka_class_ops *ops;
   struct semaphore chrdev_lock;
};

static int pka_fop_open(struct inode *inode, struct file *file)
{
   struct pka_chrdev_priv *priv;
   struct device *chrdev;

   rcu_read_lock();
   chrdev = get_device(idr_find(&pka_idr, MINOR(inode->i_rdev)));
   rcu_read_unlock();

   if (!chrdev)
      return -ENODEV;

   /*
    * In the future, it would be good to support concurrent access to the
    * device.  For now, prevent that from happening.
    */
   priv = dev_get_drvdata(chrdev);
   if (down_trylock(&priv->chrdev_lock))
      return -EAGAIN;

   file->private_data = chrdev;
   return 0;
}

static int pka_fop_release(struct inode *inode, struct file *file)
{
   struct device *chrdev = file->private_data;
   struct pka_chrdev_priv *priv = dev_get_drvdata(chrdev);

   if (priv->ops->pka_abort)
      priv->ops->pka_abort(priv->pka_dev);

   up(&priv->chrdev_lock);
   put_device(chrdev);

   return 0;
}

static long
pka_std_ioctl(struct device *chrdev, unsigned cmd, void __user *arg)
{
   struct pka_chrdev_priv *priv = dev_get_drvdata(chrdev);
   struct pka_param param;
   long rc;

   /*
    * To allow for future extension, we only require that there be as many
    * value bytes as the size parameter implies.  Currently this applies to
    * call operations, too, even though the value bytes are not used in that
    * case.
    */
   rc = copy_from_user(&param, arg, sizeof param);
   if (rc > sizeof param.value)
      return -EFAULT;
   if (param.size > sizeof param.value)
      return -EINVAL;
   if (rc > sizeof param.value - param.size)
      return -EFAULT;

   switch (cmd) {
   case PKA_IOC_SETPARAM:
      if (priv->ops->pka_setparam) {
         return priv->ops->pka_setparam(priv->pka_dev, &param);
      }
      dev_warn(priv->pka_dev, "SETPARAM not implemented\n");
      return -ENOSYS;
   case PKA_IOC_GETPARAM:
      if (priv->ops->pka_getparam) {
         rc = priv->ops->pka_getparam(priv->pka_dev, &param);
         if (rc < 0)
            return rc;
         break;
      }
      dev_warn(priv->pka_dev, "GETPARAM not implemented\n");
      return -ENOSYS;
   case PKA_IOC_CALL:
      if (priv->ops->pka_call) {
         return priv->ops->pka_call(priv->pka_dev, &param);
      }
      dev_warn(priv->pka_dev, "CALL not implemented\n");
      return -ENOSYS;
   case PKA_IOC_TESTF:
      if (param.size > 0)
         return -EINVAL;

      if (priv->ops->pka_testf) {
         return priv->ops->pka_testf(priv->pka_dev, &param);
      }
      dev_warn(priv->pka_dev, "TESTF not implemented\n");
      return -ENOSYS;
   default:
      BUG();
   }

   rc = copy_to_user(arg, &param, offsetof(struct pka_param, value)
                                  + param.size);
   if (rc != 0)
      return -EFAULT;

   return 0;
}

static long
pka_flag_ioctl(struct device *chrdev, unsigned cmd, void __user *arg)
{
   struct pka_chrdev_priv *priv = dev_get_drvdata(chrdev);
   struct pka_flag flag;
   long rc;

   rc = copy_from_user(&flag, arg, sizeof flag);
   if (rc > 0)
      return -EFAULT;

   if (flag.op >= PKA_FLAG_OP_MAX)
      return -EINVAL;

   switch (cmd) {
   case PKA_IOC_SETF:
      if (priv->ops->pka_setf) {
         return priv->ops->pka_setf(priv->pka_dev, &flag);
      }
      dev_warn(priv->pka_dev, "SETF not implemented\n");
      return -ENOSYS;
   }

   BUG();
}

static long
pka_fop_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
   struct device *chrdev = file->private_data;
   struct pka_chrdev_priv *priv = dev_get_drvdata(chrdev);

   /* Handle "simple" ioctls. */
   switch (cmd) {
   case PKA_IOC_SETPARAM:
   case PKA_IOC_GETPARAM:
   case PKA_IOC_CALL:
   case PKA_IOC_TESTF:
      return pka_std_ioctl(chrdev, cmd, (void __user *)arg);
   case PKA_IOC_SETF:
      return pka_flag_ioctl(chrdev, cmd, (void __user *)arg);
   case PKA_IOC_WAIT:
      if (priv->ops->pka_wait)
         return priv->ops->pka_wait(priv->pka_dev);
      dev_warn(priv->pka_dev, "WAIT not implemented\n");
      return -ENOSYS;
   case PKA_IOC_ABORT:
      if (priv->ops->pka_abort)
         return priv->ops->pka_abort(priv->pka_dev);
      dev_warn(priv->pka_dev, "ABORT not implemented\n");
      return -ENOSYS;
   }

   /* Handle ioctls with variable-length data. */
   switch (cmd & ~(_IOC_SIZEMASK << _IOC_SIZESHIFT)) {
   case PKA_IOC_IDENT(0):
      /* TODO: copy something resembling a device name. */
      return 0;
   }

   if (priv->ops->pka_ioctl)
      return priv->ops->pka_ioctl(priv->pka_dev, cmd, arg);

   return -ENOTTY;
}

static const struct file_operations pka_chrdev_fops = {
   .owner          = THIS_MODULE,
   .open           = pka_fop_open,
   .release        = pka_fop_release,
   .unlocked_ioctl = pka_fop_ioctl,
};

static void pka_chrdev_release(struct device *chrdev)
{
   struct pka_chrdev_priv *priv = dev_get_drvdata(chrdev);

   put_device(priv->pka_dev);
   kfree(chrdev);
   kfree(priv);
}

static struct device *
pka_chrdev_create(struct device *parent, int id, const struct pka_class_ops *ops)
{
   struct pka_chrdev_priv *priv;
   struct device *chrdev;

   priv = kmalloc(sizeof *priv, GFP_KERNEL);
   if (!priv)
      return ERR_PTR(-ENOMEM);

   *priv = (struct pka_chrdev_priv) {
      .pka_dev = get_device(parent),
      .ops = ops,
   };
   sema_init(&priv->chrdev_lock, 1);

   chrdev = device_create(pka_class, parent, MKDEV(pka_major, id),
                          priv, "pka%d", id);
   if (IS_ERR(chrdev)) {
      put_device(parent);
      kfree(priv);
   } else {
      chrdev->release = pka_chrdev_release;
   }

   return chrdev;
}

struct device *pka_chrdev_register(struct device *dev, const struct pka_class_ops *ops)
{
   struct device *chrdev;
   int rc, id;

   do {
      if (idr_pre_get(&pka_idr, GFP_KERNEL) == 0) {
         return ERR_PTR(-ENOMEM);
      }

      mutex_lock(&pka_idr_mutex);
      rc = idr_get_new(&pka_idr, NULL, &id);
      mutex_unlock(&pka_idr_mutex);
   } while (rc == -EAGAIN);

   if (rc < 0)
      return ERR_PTR(rc);

   chrdev = pka_chrdev_create(dev, id, ops);

   mutex_lock(&pka_idr_mutex);

   if (IS_ERR(chrdev))
      idr_remove(&pka_idr, id);
   else
      idr_replace(&pka_idr, get_device(chrdev), id);

   mutex_unlock(&pka_idr_mutex);

   return chrdev;
}

void pka_chrdev_unregister(struct device *chrdev)
{
   int id = MINOR(chrdev->devt);
   struct device *idr_chrdev;

   rcu_read_lock();
   idr_chrdev = idr_find(&pka_idr, id);
   rcu_read_unlock();

   BUG_ON(idr_chrdev != chrdev);

   mutex_lock(&pka_idr_mutex);
   idr_remove(&pka_idr, id);
   mutex_unlock(&pka_idr_mutex);

   synchronize_rcu();

   device_unregister(chrdev);
   put_device(idr_chrdev);
}

int __init pka_class_init(void)
{
   pka_major = register_chrdev(0, CLASSNAME, &pka_chrdev_fops);
   if (pka_major < 0)
      return pka_major;

   pka_class = class_create(THIS_MODULE, CLASSNAME);
   if (!pka_class) {
      unregister_chrdev(pka_major, CLASSNAME);
      return PTR_ERR(pka_class);
   }

   return 0;
}

void pka_class_exit(void)
{
   class_destroy(pka_class);
   unregister_chrdev(pka_major, CLASSNAME);
   idr_destroy(&pka_idr);
}
