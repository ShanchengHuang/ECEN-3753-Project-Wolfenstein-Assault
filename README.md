# ECEN-3753-Project-Wolfenstein-Assault

## This weeek : **Project: Planning**

## Task diagram

![Task diagram](/screenshot/report/ECEN-3753-Project-Wolfenstein-Assault.png)


## Test Plan and results

- Test the main path of the Task Physics function. This is because the Task Physics function is responsible for the core game logic, such as updating the platform's position and velocity, updating the satchel charge's path, and determining when the game is won or lost. Testing the main path of this function would help to ensure that the game logic is working correctly and that the game is functioning as intended.

  - One possible way to create a cutting point for testing the main path of Task Physics would be to insert a breakpoint at the beginning of the function and step through the code line by line, using a debugger to verify that each step is executing as expected. Another approach would be to use a code coverage tool to identify the most frequently executed lines of code and focus testing efforts on those lines to ensure that they are working 

- Boundary condition handling in the Task Physics function. Boundary condition handling is important because it ensures that the game functions correctly even when unexpected inputs or conditions are encountered.

  - One possible way to create a cutting point for testing boundary condition handling would be to introduce intentionally incorrect or out-of-bounds inputs to the Task Physics function and verify that the function handles these inputs correctly. For example, you could test what happens when the platform's position exceeds the boundaries of the canyon or when the satchel charge is thrown with an invalid velocity. By testing these boundary conditions, you can ensure that the game functions correctly even under unexpected circumstances.

## Statement of where your project stands
- Accurate summary statement of your functionality deliverables and usability so far
  - (Week 1) This week I did the project planning, created my task diagram, and identified 2 cutting points for unit testing
- Summary effort & estimate numbers
  - (Week 1)I have completed 10% of my currently-scoped which in 5 hr (Which would take from 75 to 150 hr), estimated work 15% of the budgeted total-project time next week.  For the work that has been completed, I took 1.5x as much time as I estimated.

## List of in-scope work items

### Next Task to work on:

Implement Task Button - This task was not and need to be done. The implementation was straightforward, and it should completed within the estimated time frame.

Implement Task CapSense - This task was not and need to be done. The implementation was relatively simple, and it should completed within the estimated time frame.

Implement ISR Button - This task was not and need to be done. The implementation was straightforward, and it should completed within the estimated time frame.

Implement ISR CapSense - This task was not and need to be done. The implementation was relatively simple, and it should completed within the estimated time frame.

Implement Queue Button - This task was not and need to be done. The implementation was straightforward, and it should completed within the estimated time frame.

Implement Queue CapSense - This task was not and need to be done. The implementation was relatively simple, and it should completed within the estimated time frame.

Implement Semaphore Button - This task was not and need to be done. The implementation was straightforward, and it should completed within the estimated time frame.

Implement Semaphore CapSense - This task was not and need to be done. The implementation was relatively simple, and it should completed within the estimated time frame.

Physics model. [Platform] - This task was not completed . The implementation of the physics model for the platform was more challenging than anticipated, and it may took longer than estimated. In hindsight, it would have been helpful to have a menu system in place to make it easier to test the physics model with different inputs.

### Ongoing work items:

Implement Task Physics - This task is currently in progress. The implementation of this task involves updating the platform's position and velocity, updating the satchel charge's path, and determining when the game is won or lost. The implementation of the physics calculations is taking longer than expected, and additional testing and debugging will be required.

Implement Task Display - This task is not yet complete. The implementation of this task involves processing information necessary to update the LCD and updating the LED controls. The implementation is expected to take approximately 4 hours.

Implement Queue Physics - This task is currently in progress. The implementation of this task involves queuing up updates to the platform's position and velocity, the satchel charge's path, and other game parameters. The implementation is expected to take approximately 2 hours.

Implement LCD display update in Task Display - This task is not yet complete. The implementation of this task involves updating the LCD with the current game state. The implementation is expected to take approximately 3 hours.

Implement LED brightness update in Task Display - This task is not yet complete. The implementation of this task involves updating the brightness of the LED to show the current force magnitude. The implementation is expected to take approximately 1 hour.

Implement boundary condition handling in Task Physics - This task is not yet complete. The implementation of this task involves handling boundary conditions such as the platform going out of bounds or the satchel charge being thrown with an invalid velocity. The implementation is expected to take approximately 6 hours.

Implement satchel charge throwing logic - This task is not yet complete. The implementation of this task involves generating a random position for the satchel charge to land and calculating the velocity required to reach that position. The implementation is expected to take approximately 5 hours.

Test and debug entire system - This task is not yet complete

### Works that is done: 

Design data structures - This task was done. The data structures were designed based on the project requirements, and the implementation was straightforward.



<!-- Note: Completed work items are highlighted in bold. -->

## Risk Register

| Risk Description | Category | Likelihood | Impact | Mitigation | Status |
| --- | --- | --- | --- | --- | --- |
| Implementation delays due to lack of experience with certain technologies | Technical | Medium | High | Perform thorough research on technologies before beginning implementation. Consult with instructors and peers for guidance. | Ongoing |
| Having dead lock with in the physics queue| Technical | High | High | Carefully follow the design of the flow chart, confirm the design and rationality of the flow chart to TA, and the professor | Ongoing |
| Unexpected changes to project requirements | Project | Low | High | Ensure project requirements are clearly defined and documented. Allow for flexibility in project plan to accommodate changes. | Ongoing |
| Inadequate testing leading to undetected bugs | Technical | Medium | High | Develop a comprehensive testing plan to ensure all features are thoroughly tested. Utilize automated testing tools where possible. | Resolved |
| Difficulties integrating different software components | Technical | Medium | High | Use well-documented interfaces between components. Perform integration testing early and frequently. | Ongoing |
<!-- | Hardware malfunctions or failures | Technical | Low | High | Test hardware components thoroughly before use. Have backup hardware available in case of failures. | Ongoing | -->
| Inaccurate estimates of required time and effort | Project | Medium | Medium | Break down tasks into smaller, more manageable pieces. Consult with team members for second opinions on estimates. Track progress regularly to identify areas where more time is needed. | Ongoing |


This week, I am resolving the risk of inadequate testing leading to undetected bugs by developing a comprehensive testing plan. I will continue to ROAM the risks each week and update the risk register accordingly.