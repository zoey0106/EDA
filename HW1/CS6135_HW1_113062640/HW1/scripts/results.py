import random
import numpy as np
import pandas as pd
import os
import re
def extract_value_from_report(pattern, filename):
    """
    Extract numerical value from Innovus reports.
    """
    if not os.path.exists(filename):
        print(f'file not found!filename: {filename}')
        return -1
    
    with open(filename, 'r') as file:
        for line in file:
            match = re.search(pattern, line, re.IGNORECASE)
            if match:
                return float(match.group(1))
        return float("inf") 
    
if __name__ == "__main__":
        core_utilization = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
        clock_period = [1100 ,1000, 800, 600, 400, 300, 200]
        data = []
        for CU in core_utilization:
            for CP in clock_period:
                
                summary_file_name = f"../summary_{CU}_{CP}.rpt"
                timing_file_name = f"../timing_{CU}_{CP}.rpt"
                drc_file_name = f"../drc_{CU}_{CP}.rpt"

                wire_length = extract_value_from_report(r"Total wire length:\s*([\d\.]+)", summary_file_name)
                chip_area = extract_value_from_report(r"Total area of chip:\s*([\d\.]+)", summary_file_name)
                slack_time = extract_value_from_report(r"Slack time:\s*([\d\.-]+)", timing_file_name)
                drc_errors = extract_value_from_report(r"Verification Complete\s*:\s*(\d+)\s+viols?", drc_file_name)

                data.append([CU, CP, wire_length, chip_area, slack_time, drc_errors])
# Convert to a Pandas DataFrame
df = pd.DataFrame(data, columns=["Core Utilization", "Clock Period", "Wire Length", "Chip Area", "Slack Time", "DRC Errors"])

csv_filename = "HW1_results.csv"
df.to_csv(csv_filename)
