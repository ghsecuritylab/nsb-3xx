/*
 * Copyright (c) 2013 Elliptic Technologies Inc.
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

#include "elpspacc.h"

/* Read the IRQ status register and process as needed */

uint32_t spacc_process_irq(spacc_device *spacc)
{
   uint32_t temp;
   int x, cmd_max;
   unsigned long lock_flag;

   PDU_LOCK(&spacc->lock, lock_flag);

   temp = pdu_io_read32(spacc->regmap + SPACC_REG_IRQ_STAT);

   /* clear interrupt pin and run registered callback */
   if (temp & SPACC_IRQ_STAT_STAT) {
      SPACC_IRQ_STAT_CLEAR_STAT(spacc);
      if (spacc->op_mode == SPACC_OP_MODE_IRQ) {
         spacc->config.fifo_cnt <<= 2;
         if (spacc->config.fifo_cnt >= spacc->config.stat_fifo_depth) {
            spacc->config.fifo_cnt = spacc->config.stat_fifo_depth;
         } 
         spacc_irq_stat_enable(spacc, spacc->config.fifo_cnt); // update fifo count to allow more stati to pile up
         spacc_irq_cmdx_enable(spacc, 0, 0);            // reenable CMD0 empty interrupt
      } else if (spacc->op_mode == SPACC_OP_MODE_WD) {
      }
      if (spacc->irq_cb_stat != NULL){
         spacc->irq_cb_stat(spacc);
      }
   }

   /* Watchdog IRQ */
   if (spacc->op_mode == SPACC_OP_MODE_WD) {
      if (temp & SPACC_IRQ_STAT_STAT_WD) {
         if (++(spacc->wdcnt) == SPACC_WD_LIMIT) {
            ELPHW_PRINT("spacc_process_irq::Hit SPACC WD LIMIT aborting WD IRQs (%08lx) (this happens when you get too many IRQs that go unanswered) \n", temp);
            ELPHW_PRINT("spacc_process_irq::Current IRQ_EN settings 0x%08lx\n", pdu_io_read32(spacc->regmap + SPACC_REG_IRQ_EN));
            spacc_irq_stat_wd_disable(spacc);
            spacc_irq_stat_enable(spacc, 1); // we set the STAT CNT to 1 so that every job generates an IRQ now
            spacc->op_mode = SPACC_OP_MODE_IRQ;
            ELPHW_PRINT("spacc_process_irq::New IRQ_EN settings 0x%08lx\n", pdu_io_read32(spacc->regmap + SPACC_REG_IRQ_EN));
         } else {
            // if the timer isn't too high lets bump it up a bit so as to give the IRQ a chance to reply
            if (spacc->config.wd_timer < (0xFFFFFFUL >> 4)) {
               spacc_set_wd_count(spacc, spacc->config.wd_timer << 4);
            }
         }

         SPACC_IRQ_STAT_CLEAR_STAT_WD(spacc);
         if (spacc->irq_cb_stat_wd != NULL) {
            spacc->irq_cb_stat_wd(spacc);
         }
      }
   }


   if (temp & SPACC_IRQ_STAT_RC4_DMA) {
      SPACC_IRQ_STAT_CLEAR_RC4_DMA(spacc);
      if (spacc->irq_cb_rc4_dma != NULL){
         spacc->irq_cb_rc4_dma(spacc);
      }
   }


   if (spacc->op_mode == SPACC_OP_MODE_IRQ && !spacc->config.is_hsm_shared) {
      cmd_max = (spacc->config.is_qos ? SPACC_CMDX_MAX_QOS : SPACC_CMDX_MAX);
      for (x = 0; x < cmd_max; x++){
         if (temp & SPACC_IRQ_STAT_CMDX(x)) {
             spacc->config.fifo_cnt = 1;
             spacc_irq_cmdx_disable(spacc, x); // disable CMD0 interrupt since STAT=1
             spacc_irq_stat_enable (spacc, spacc->config.fifo_cnt); // reset STAT count to 1

            SPACC_IRQ_STAT_CLEAR_CMDX(spacc, x);
            /* run registered callback */
            if (spacc->irq_cb_cmdx != NULL){
               spacc->irq_cb_cmdx(spacc, x);
            }
         }
      }
   }


   PDU_UNLOCK(&spacc->lock, lock_flag);

   return temp;
}

