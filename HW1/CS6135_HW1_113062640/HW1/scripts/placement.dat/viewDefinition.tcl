if {![namespace exists ::IMEX]} { namespace eval ::IMEX {} }
set ::IMEX::dataVar [file dirname [file normalize [info script]]]
set ::IMEX::libVar ${::IMEX::dataVar}/libs

create_library_set -name tc\
   -timing\
    [list ${::IMEX::libVar}/mmmc/asap7sc7p5t_AO_LVT_TT_nldm_211120.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_INVBUF_LVT_TT_nldm_220122.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_OA_LVT_TT_nldm_211120.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_SEQ_LVT_TT_nldm_220123.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_SIMPLE_LVT_TT_nldm_211120.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_AO_SLVT_TT_nldm_211120.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_INVBUF_SLVT_TT_nldm_220122.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_OA_SLVT_TT_nldm_211120.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_SEQ_SLVT_TT_nldm_220123.lib\
    ${::IMEX::libVar}/mmmc/asap7sc7p5t_SIMPLE_SLVT_TT_nldm_211120.lib]
create_rc_corner -name rc_typ_25\
   -preRoute_res 1\
   -postRoute_res 1\
   -preRoute_cap 1\
   -postRoute_cap 1\
   -postRoute_xcap 1\
   -preRoute_clkres 0\
   -preRoute_clkcap 0\
   -T 25\
   -qx_tech_file ${::IMEX::libVar}/mmmc/rc_typ_25/qrcTechFile_typ03_scaled4xV06
create_delay_corner -name delay_tc\
   -library_set tc\
   -rc_corner rc_typ_25
create_constraint_mode -name sdc\
   -sdc_files\
    [list /dev/null]
create_analysis_view -name view_tc -constraint_mode sdc -delay_corner delay_tc
set_analysis_view -setup [list view_tc] -hold [list view_tc]
