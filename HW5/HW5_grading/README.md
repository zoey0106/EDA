# Homework 5 Grading Script
We will use this script to grade your program. **Make sure your program can be executed by this script.**

## Preparing
* Step1:  
    Enter the `HW5_grading` directory and create a new directory named with your student ID in the `student` directory.
    ```sh
    $ cd HW5_grading/
    $ mkdir student/${your_student_id}
    ```
    For example,
    ```sh
    $ cd HW5_grading/
    $ mkdir student/113000000
    ```

* Step2:  
    Put your compressed file in the directory which you just created.  
    The correct path should be:
    ```
    HW5_grading/student/${your_student_id}/CS6135_HW5_${your_student_id}.tar.gz
    ```
    For example,
    ```
    HW5_grading/student/113000000/CS6135_HW5_113000000.tar.gz
    ```

### Notice:  
**Please make sure not to put your original directory here**, as it will remove all directories before unzipping the compressed file.

## Grading
* Step1:  
    Navigate to the `HW5_grading` directory and run `HW5_grading.sh`.
    ```sh
    $ cd HW5_grading/
    $ bash HW5_grading.sh
    ```

* Step2:  
    Check your output. Ensure the status of each checking item is **yes**.
    * If the status of a testcase is **success**, it means your program finished in time, and the output result is legal.
        ```
        host name: ic21
        compiler version: g++ (GCC) 9.3.0
        
        grading on 113000000:
         checking item           | status
        -------------------------|--------
         correct tar.gz          | yes
         correct file structure  | yes
         have README             | yes
         have Makefile           | yes
         correct make clean      | yes
         correct make            | yes
        
          testcase | total disp. |  max disp. |    runtime | status
        -----------|-------------|------------|------------|--------
           public1 |     5808743 |       2485 |       0.03 | success
           public2 |    29266300 |       3835 |       0.13 | success
           public3 |    45982073 |       5017 |       0.16 | success
           public4 |     3299123 |        217 |       0.96 | success
           public5 |     5182535 |        151 |       2.14 | success
        ```

    * If the status of a testcase is not **success**, it means your program failed in this testcase.
        ```
        host name: ic21
        compiler version: g++ (GCC) 9.3.0
        
        grading on 113000000:
         checking item           | status
        -------------------------|--------
         correct tar.gz          | yes
         correct file structure  | yes
         have README             | yes
         have Makefile           | yes
         correct make clean      | yes
         correct make            | yes
        
          testcase | total disp. |  max disp. |    runtime | status
        -----------|-------------|------------|------------|--------
           public1 |     5808743 |       2485 |       0.03 | success
           public2 |    29266300 |       3835 |       0.13 | success
           public3 |    45982073 |       5017 |       0.18 | success
           public4 |         N/A |        N/A |        TLE | Time out while testing public4.
           public5 |         N/A |        N/A |        TLE | Time out while testing public5.
        ```
