Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

**1. How much current does the system draw (instantaneous measurement) when a single LED is on with the GPIO pin set to StrongAlternateStrong?**
   Answer:5.38mA


**2. How much current does the system draw (instantaneous measurement) when a single LED is on with the GPIO pin set to WeakAlternateWeak?**
   Answer:5.37mA


**3. Is there a meaningful difference in current between the answers for question 1 and 2? Please explain your answer, 
referencing the [Mainboard Schematic](https://www.silabs.com/documents/public/schematic-files/WSTK-Main-BRD4001A-A01-schematic.pdf) and [AEM Accuracy](https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf) section of the user's guide where appropriate. Extra credit is avilable for this question and depends on your answer.**
   
   Answer: GPIO drive strength is the maximum current that can be drawn while the logic voltage level remains unchanged.  Strong means it is allowed to draw more current than weak in the GPIO and it does this by changing output impedance. 
According to this thread, when the drive strength is strong and IOVDD voltage is greater than 3V the maximum current that GPIO can draw is 20 mA and 3mA when it is set to weak. With more current flow to the load circuit and same voltage, the power consumption will expect to be higher. However, question 1 and 2's current does not seem to have too much difference this is probably because the LED is drawing less than 3mA, so whether the setting is strongalternatestrong or weakalternateweak, LED is drawing the similar amount of current which is less than 3mA. 



**4. With the WeakAlternateWeak drive strength setting, what is the average current for 1 complete on-off cycle for 1 LED with an on-off duty cycle of 50% (approximately 1 sec on, 1 sec off)?**
   Answer:5.09mA


**5. With the WeakAlternateWeak drive strength setting, what is the average current for 1 complete on-off cycle for 2 LEDs (both on at the time same and both off at the same time) with an on-off duty cycle of 50% (approximately 1 sec on, 1 sec off)?**
   Answer:5.11mA


