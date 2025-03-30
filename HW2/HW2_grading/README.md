# Homework 2 Grading Script
We will use this script to grade your program. **Make sure your program can be executed by this script.**

## Preparing
* Step1:  
    Enter the `HW2_grading` directory and create a new directory named with your student ID in the `student` directory.
    ```sh
    $ cd HW2_grading/
    $ mkdir student/${your_student_id}
    ```
    For example,
    ```sh
    $ cd HW2_grading/
    $ mkdir student/113000000
    ```

* Step2:  
    Put your compressed file in the directory which you just created.  
    The correct path should be:
    ```
    HW2_grading/student/${your_student_id}/CS6135_HW2_${your_student_id}.tar.gz
    ```
    For example,
    ```
    HW2_grading/student/113000000/CS6135_HW2_113000000.tar.gz
    ```

### Notice:
**Please make sure not to put your original directory here**, as it will remove all directories before unzipping the compressed file.

## Grading
* Step1:  
    Navigate to the `HW2_grading` directory and run `HW2_grading.sh`.
    ```sh
    $ cd HW2_grading/
    $ bash HW2_grading.sh
    ```

* Step2:  
    Check your output. Ensure the status of each checking item is **yes**.
    * If the status of a testcase is **success**, it means your program finished in time, and the output result is legal.
        ```
        host name: ic21
        compiler version: g++ (GCC) 9.3.0
        
        grading on 113000000:
         checking item          | status
        ------------------------|--------
         correct tar.gz         | yes
         correct file structure | yes
         have README            | yes
         have Makefile          | yes
         correct make clean     | yes
         correct make           | yes
        
          testcase |   cut size |    runtime | status
        -----------|------------|------------|--------
           public1 |       5951 |       0.28 | success
           public2 |       4685 |       0.41 | success
           public3 |      95117 |       2.70 | success
           public4 |     332873 |       1.97 | success
           public5 |     600040 |      14.12 | success
           public6 |     436883 |       7.06 | success
        ```

    * If the status of a testcase is not **success**, it means your program failed in this testcase.
        ```
        host name: ic21
        compiler version: g++ (GCC) 9.3.0
        
        grading on 113000000:
         checking item          | status
        ------------------------|--------
         correct tar.gz         | yes
         correct file structure | yes
         have README            | yes
         have Makefile          | yes
         correct make clean     | yes
         correct make           | yes
        
          testcase |   cut size |    runtime | status
        -----------|------------|------------|--------
           public1 |       5951 |       0.23 | success
           public2 |       4685 |       0.28 | success
           public3 |        N/A |        TLE | Time out while testing public3.
           public4 |        N/A |        TLE | Time out while testing public4.
           public5 |        N/A |        TLE | Time out while testing public5.
           public6 |        N/A |        TLE | Time out while testing public6.
        ```
