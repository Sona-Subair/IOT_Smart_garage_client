Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.* 

1. What is the average current per period?

   Answer:20.34uA
   
   <br>Screenshot:  
   ![Avg_current_per_period](https://github.com/CU-ECEN-5823/ecen5823-assignment4-JiabinLin12/blob/master/img/A4/period.png)  

2. What is the average current when the Si7021 is Powered Off?

   Answer:2.89uA
   
   <br>Screenshot:  
   ![Avg_current_LPM_Off](https://github.com/CU-ECEN-5823/ecen5823-assignment4-JiabinLin12/blob/master/img/A4/sensorofftime.png)  

3. What is the average current when the Si7021 is Powered On?

   Answer:491.38uA
   
   <br>Screenshot:  
   ![Avg_current_LPM_On](https://github.com/CU-ECEN-5823/ecen5823-assignment4-JiabinLin12/blob/master/img/A4/sensorontime.png)  

4. How long is the Si7021 Powered On for 1 temperature reading?\

   Answer:99.80ms
   
   <br>Screenshot:  
   ![duration_lpm_on](https://github.com/CU-ECEN-5823/ecen5823-assignment4-JiabinLin12/blob/master/img/A4/sensorontime.png)  

5. Compute what the total operating time of your design for assignment 4 would be in hours, assuming a 1000mAh battery power supply?\
   Answer:
   1 hours = 3600s \
   Total current use per hour = (3600/3) * 20.34uA = 24408uA = 24.408mA \
   Total operating time = 1000mAh / 24.408mA = 41h 
   
   
6. How has the power consumption performance of your design changed since the previous assignment?\
   Answer:
   
   The average current per period is around 154.47uA. Since voltage are the same between two assignment, A4 is around 155/20 = 7.75 times more power efficient than A3 
   
7. Describe how you have tested your code to ensure you are sleeping in EM1 mode during I2C transfers.\
   Answer:
   
   Refer to A2, when the device sleep in EM0 the current is around 4.8mA and the current consumtion for sleeping in EM2 is around 4.7uA. As the screenshot is shown, we have around 245uA after I2C0 setup, this value is between the range of EM2 and EM0, so it has to be sleeping in EM1. 
   
   <br>Screenshot:  
   ![verification](https://github.com/CU-ECEN-5823/ecen5823-assignment4-JiabinLin12/blob/master/img/A4/verification.png)  
