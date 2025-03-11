for CU in 0.6; do
    for CP in 575; do
        FILE_NAME=_${CU}_${CP}
        sed -i "8s/.*/create_clock -name \"clk\" -period ${CP} [get_ports clk]/" ../sdc/sha256.sdc
        sed -i "80s/.*/floorPlan -coreMarginsBy die -site asap7sc7p5t -r 1.0 ${CU} 6.22 6.22 6.22 6.22/" apr.tcl
        sed -i "144s,.*,report_timing > ../timing${FILE_NAME}.rpt," apr.tcl
        sed -i "143s,.*,summaryReport -noHtml -outfile ../summary${FILE_NAME}.rpt," apr.tcl
        sed -i "145s,.*,verify_drc > ../drc${FILE_NAME}.rpt," apr.tcl

        innovus -init apr.tcl 
        echo "file name: $FILE_NAME"
        grep -i "total wire length" ../summary${FILE_NAME}.rpt
        grep -i "total area of chip" ../summary${FILE_NAME}.rpt
        grep -i "Slack time" ../timing${FILE_NAME}.rpt
        grep -i "Verification Complete" ../drc${FILE_NAME}.rpt
    done
done

