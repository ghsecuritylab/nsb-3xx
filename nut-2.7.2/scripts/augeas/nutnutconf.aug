(*
Module: NutNutConf
 Parses /usr/etc/nut.conf

Author: Frederic Bohe  <fredericbohe@eaton.com>

About: License
  This file is licensed under the GPL.

About: Lens Usage
  Sample usage of this lens in augtool

    * Print NUT MODE start-up configuration:
      > print /files/usr/etc/nut.conf/MODE

About: Configuration files
  This lens applies to /usr/etc/nut.conf. See <filter>.
*)

module NutNutConf =
  autoload nut_xfm


(************************************************************************
 * Group:                 NUT.CONF
 *************************************************************************)

(* general *)
let def_sep  = IniFile.sep IniFile.sep_re IniFile.sep_default
let sep_spc  = Util.del_opt_ws ""
let eol      = Util.eol
let comment  = Util.comment
let empty    = Util.empty


let nut_possible_mode = "none"
			| "standalone"
			| "netserver"
			| "netclient"

let nut_mode = [ sep_spc . key "MODE" . def_sep . sep_spc . store nut_possible_mode . eol ]

let nut_lns  = (nut_mode|comment|empty)*

let nut_filter = ( incl "/usr/etc/nut.conf" )
			. Util.stdexcl

let nut_xfm    = transform nut_lns nut_filter
