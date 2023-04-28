# ECEN-3753-Project-Wolfenstein-Assault

## This weeek : **Week 5 Unit Testing, final implementation.**

## Task diagram

![Task diagram](/screenshot/report/ECEN-3753-Project-Wolfenstein-Assault.png)

1. Start
    |
2. Initialize System Clock, GPIO, and other peripherals
    |
3. Initialize the LCD screen and related components
    |
4. Draw the side boundaries of the game area
    |
5. Enter the main loop
    |        
6.  Update the game state (move satchel, platform, and railgun)
    |    |--> 6.1 Move satchel based on its velocity
    |    |--> 6.2 Update platform position based on user input
    |    |--> 6.3 Calculate and update railgun angle based on satchel position
    |
7.  Check for game events (collisions, game over, or paused)
    |    |--> 7.1 Check for satchel collisions with boundaries or railgun
    |    |--> 7.2 Check for game over conditions (e.g., satchel falling off the screen)
    |    |--> 7.3 Handle pause/resume game events
    |
8.  Draw the game elements on the LCD screen
    |    |--> 8.1 Draw the platform and railgun
    |    |--> 8.2 Draw the satchel as a square
    |    |--> 8.3 Draw any other game elements (e.g., scores or indicators)
    |    |--> 8.4 Update the LCD display
    |
9.  Wait for a predefined time period before updating the game state
    |
10. Repeat steps 6 through 9 until the game is stopped or the user quits
    |
11. End


## Test Plan and results (Week5 updated)

- Test the main path of the Task Physics function. This is because the Task Physics function is responsible for the core game logic, such as updating the platform's position and velocity, updating the satchel charge's path, and determining when the game is won or lost. Testing the main path of this function would help to ensure that the game logic is working correctly and that the game is functioning as intended.

  - One possible way to create a cutting point for testing the main path of Task Physics would be to insert a breakpoint at the beginning of the function and step through the code line by line, using a debugger to verify that each step is executing as expected. Another approach would be to use a code coverage tool to identify the most frequently executed lines of code and focus testing efforts on those lines to ensure that they are working 

- Boundary condition handling in the Task Physics function. Boundary condition handling is important because it ensures that the game functions correctly even when unexpected inputs or conditions are encountered.

  - One possible way to create a cutting point for testing boundary condition handling would be to introduce intentionally incorrect or out-of-bounds inputs to the Task Physics function and verify that the function handles these inputs correctly. For example, you could test what happens when the platform's position exceeds the boundaries of the canyon or when the satchel charge is thrown with an invalid velocity. By testing these boundary conditions, you can ensure that the game functions correctly even under unexpected circumstances.
  
- Test all flags, interrupts and mutexes in all tasks to ensure all resources are available to all functions and prevent any deadlocks from occurring (Week 3 updated)

  - Unit test and pass all the flag
  - Unit test and all the metux are working
  - Unit test all the timer functions
  - Unit test share files

- Based on the current framework I have built, I can have some simple unit test for all the flag, mutex, and interrupt, I can change the values of a variables and use debug to check.(Week4)

- Test LCD task, useing the Lab 7 as unit test, so I can test and design all the display element without interfere with other code (Week 5)
![](/screenshot/report/LCD_actual.jpg)
![](/screenshot/report/LCD_design_1.jpg)
![](/screenshot/report/LCD_design_2.jpg)

- Testing the movement of the platform (Week 5)
- Testing the satchel movement(Week 5)
- 


## Statement of where your project stands (Week4 updated)

|        | Est hr | %of the Est | Actural Effeort | Status |
|--------|--------|-------------|-----------------|--------|
| Week 1 | 6      | 10          | 5 hr            | 10%    |
| Week 2 | 5      | 20%         | 5 hr            | 20%    |
| Week 3 | 5      | 40%         | 6 hr            | 40%    |
| Week 4 | 8      | 70%         | 5-6hr           | 65%    |
| Week 5 | 10     | 100%        | 12hr            | 85%    |

- Accurate summary statement of your functionality deliverables and usability so far
  - (Week 1) This week I did the project planning, created my task diagram, and identified 2 cutting points for unit testing
  - (Week 2) This week I have started coding the project, and useing the Lab7 workspace as the template and start working base on it, this week what I have mostly done is set up the function framework in the header file, start implementing some important files like physics, and set up the create date base of the queue. And doing some unit tests for the mutex.
  - (Week 3) For this week, the plan is update all the flags and the Mutex, also finish some input function as `Button_Task` and `CapSensense_Task`, and start doing the `Physics_Task` and it's queue.
  - (Week 4) For this week, I should have finished or come close to finish all the other functions but the `Physics_Task`  
  - (Week 5) This is the last week before the official submission. This week I mainly modify bugs. Most of the game has been completed. However, due to the temporary modification of the game structure in the previous week, there are problems in the final integration part.
- Summary effort & estimate numbers
  - (Week 1)I have completed 10% of my currently-scoped which in 5 hr (Which would take from 75 to 150 hr), estimated work 15% of the budgeted total-project time next week.  For the work that has been completed, I took 1.5x as much time as I estimated.
  - (Week 2) For this week I have completed 20ish% of the currently scoped, in about 5-5hr, in estimated 20% of the total work hours. Which is slightly faster than the initial estimate
  - (Week 3) For this week I have completed 40ish% of the currently scoped, in about 6hr, in estimated 35-40% of the total work hours. Which is slightly slower than the initial estimate
  - (Week 4) For this week I have completed 65ish% of the currently scoped, in about 5hr, in estimated 60% of the total work hours. Which is slightly slower than the initial estimate
  - (Week 5) This week I spent far more than 10 hours of working time, about 15 hours, 50% more than planned, 85% of the total project completed, And I have test LCD display test and other smaller tests, 20% behind the expected plan.

## List of in-scope work items

### Next Task to work on: (Week 5 updated)


Physics model. [Platform] - This task was not completed . The implementation of the physics model for the platform was more challenging than anticipated, and it may took longer than estimated. In hindsight, it would have been helpful to have a menu system in place to make it easier to test the physics model with different inputs.

Implement boundary condition handling in Task Physics - This task is not yet complete. The implementation of this task involves handling boundary conditions such as the platform going out of bounds or the satchel charge being thrown with an invalid velocity. The implementation is expected to take approximately 6 hours.

Implement satchel throwing logic - This task is not yet complete. The implementation of this task involves generating a random position for the satchel charge to land and calculating the velocity required to reach that position. The implementation is expected to take approximately 5 hours.


Test and debug entire system - This task is not yet complete

### Ongoing work items (Week5 updated):

Implement Queue Button - This task was almost done. The implementation was straightforward, and it should completed within the estimated time frame.
ssssssssssssssss
Implement Semaphore Button - This task was almost done. The implementation was straightforward, and it should completed within the estimated time frame.

Implement Semaphore CapSense - This task was almost done. The implementation was relatively simple, and it should completed within the estimated time frame.

Implement LCD Task - This task was just started. The implementation would have problem, I should 

Implement Task Physics - This task is currently in progress. The implementation of this task involves updating the platform's position and velocity, updating the satchel charge's path, and determining when the game is won or lost. The implementation of the physics calculations is taking longer than expected, and additional testing and debugging will be required.

Implement Task Display - This task is currently in progress. The implementation of this task involves processing information necessary to update the LCD and updating the LED controls. The implementation is expected to take approximately 4 hours.

Implement Queue Physics - This task is currently in progress. The implementation of this task involves queuing up updates to the platform's position and velocity, the satchel charge's path, and other game parameters. The implementation is expected to take approximately 2 hours.


### Works that is done: 

Implement Task Button - This task was done, just need integrate with other functions. The implementation was straightforward, and it should completed within the estimated time frame.

Implement Task CapSense - This task was done, just need integrate with other functions. The implementation was relatively simple, and it should completed within the estimated time frame.

Implement ISR Button - This task was done, just need integrate with other functions. The implementation was straightforward, and it should completed within the estimated time frame.

Implement LED Task - This task was done, just need integrate with other functions. The implementation was straightforward, and it should completed within the estimated time frame.

Design data structures - This task was done. The data structures were designed based on the project requirements, and the implementation was straightforward.

Finishing the base function framework - and should have the all the post pend in place, however, the function is not complet, there still have work need to do, but we have great start



<!-- Note: Completed work items are highlighted in bold. -->

## Risk Register (Week5 updated)

<!-- Adding issue of include in all the task file, eg: input.c has os define from app.h -->

| Risk Description | Category | Likelihood | Impact | Mitigation | Status |
| --- | --- | --- | --- | --- | --- |
| Implementation delays due to lack of experience with certain technologies | Technical | Medium | High | Perform thorough research on technologies before beginning implementation. Consult with instructors and peers for guidance. | Ongoing |
| Having dead lock with in the physics queue| Technical | High | High | Carefully follow the design of the flow chart, confirm the design and rationality of the flow chart to TA, and the professor | Ongoing |
| Inadequate testing leading to undetected bugs | Technical | Medium | High | Develop a comprehensive testing plan to ensure all features are thoroughly tested. Utilize automated testing tools where possible. | Ongoing |
| Inaccurate estimates of required time and effort | Project | Medium | Medium | Break down tasks into smaller, more manageable pieces. Consult with team members for second opinions on estimates. Track progress regularly to identify areas where more time is needed. | Ongoing |
| Failed to implement physics simulation formula in game | Technical | Medium | High | The first step is to identify where the problem lies. Is the formula not working at all? Is it producing unexpected results? Are there errors in the code? | Ongoing |
| Difficulties integrating different software components | Technical | Medium | High | Use well-documented interfaces between components. Perform integration testing early and frequently. | Ongoing |
| Unexpected changes to project requirements | Project | Low | High | Ensure project requirements are clearly defined and documented. Allow for flexibility in project plan to accommodate changes. | Resolved |
| Include necessary files in all task files | Project | Low | Medium | Ensure that all task files include the necessary files to avoid unexpected errors or issues. Communicate the importance of this to the project team. | In progress |


This week, I am resolving the risk of inadequate testing leading to undetected bugs by developing a comprehensive testing plan. I will continue to ROAM the risks each week and update the risk register accordingly.