---
layout: page
title: Results
permalink: /results/
---
# Desired Specifications and Key Performance Aspects
- Successfully tracks user input from the Wii Nunchuk
   - MET! The MCU successfully loads in data bytes from the Nunchuk relating to the sensors on the controller.
- Wrote a library for initializing and driving the LED matrix with the FPGA
  - MET! See code in the Documentation tab.
- Wrote a library for interfacing with the Wii Nunchuk adapter over I2C using the MCU
  - MET! See code in the Documentation tab.
- Display a maze on the LED matrix with wall collision physics
  - MET! The MCU handles the wall collision physics by checking intended player movements vs player position. During play, the player cannot phase through walls or outisde of the maze.
- Communicates user input from Wii Nunchuk to the LED matrix
  - MET! The MCU successfully decodes data from the Nunchuk, updates and sends game/player data to the FPGA, and the FPGA sends the proper display data to the LED matrix.
- Winning/Losing animations play on LED matrix
  - MET! See animations section in the Design tab.
- Project is operational for a presentation to peers/professor
  - MET!
- Created a website clearly displays project details/results
  - MET!

# Results
With the completion of the specifications above, we were able to create a playable maze game inside its own game cabinet. The maze game has a timer that introduces a win/lose condition and allows the player to navigate the maze using the Wii Nunchuk controller. The LED matrix displays the maze and plays all our fun animations properly. 

[See our design in action!](https://youtu.be/Vm8TDvq7yHE)

However, there are some limitations. With the current design, the display is not being fully refreshed each time the game state updates. Rather, only the sections of the board that have changed are updated. The FPGA design does not update the board quickly enough to perform a full-board refresh without flickering. For reference, clearing the entire board takes an entire 200 milliseconds. This limitation isn't an issue for a maze game where only the player's position changes throughout the game, but will prevent this display driver from smoothly running more complex games, such as anything involving 3D rendering. 
