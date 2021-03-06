# matrixGame
1. Task Requirements
Build a GAME using an LCD screen, 16 LED Matrix, and a Joystick. The game should be fun, not too easy, with different levels of difficulty and sound! 
All Hardware features must be customizable (ex brightness), all important data must be held on EEPROM, top 3 high scores must be presented, LCD screen must display some info at any stage of the game.

Must be screens: 
  - playing (the actual game)
  - about (simple info about creator)
  - settings (set difficulty, player's name, screen brightness, etc)
  - highscore (top 3 so far)
  - game over (message score)
  - new highscore (message + score)
  
2. Picture of the setup 
![matrix](https://user-images.githubusercontent.com/61291904/147041469-e8938dee-e35e-4bb3-b811-ba85073e1e86.png)

3. Video showcasing functionality
https://www.youtube.com/watch?v=BxTgqT_m-Rk

4. Used components:
    - Arduino UNO
    - LCD screen
    - 16 LED Matrix
    - Buzzer
    - Joystick
    - Shift register
    - a lot of cables 
    - some < 330 ohm rezistors
    
5. Mega Tiles
  Purpose: Acquire as many tiles as fast as possible, by clicking on them. The more You play, the faster everything goes. As you progress through difficulties the game gets harder, by providing long click tiles. Have fun while listening to the best song themes, like DOOM!
  
  - The main menu displays the principal options, like Play, Settings, Highscore, and About. 
  - About screen displays the creator's name (me :) ), GitHub profile, and the game's name.
  - Highscore screen, obviously shows the top 3 high scores and the players' names. You can reset them anytime.
  - Settings provides customization of the entire setup: name, brightness for LCD and Matrix, contrast, and difficulty.
  - PLay state displays the game on the Matrix and the status on the LCD (lives, score, and difficulty).
  - Game over only displays the score and the "Game Over" message
  - New high score only displays the score and the "New Highscore" message

6. Code thinking process
- LCD and Matrix are taken apart so that they can't interfere with each other. 
- LCD screens are managed with lcdState, making the process as clear as possible. Each time the state changes, the screen is cleared and the new one is displayed.
- The game in projected in 2 parts, player input and tiles management. Tiles are spawned at rendom delays, so that the game doesn't become cyclic. Tiles are managed by states: not spawned, active, and ready to spawn.
- For a difficulty higher than 2 some tiles are selected randomly to require a long press in order to be colected.
- As the player colects more tiles and time passes the speed gets faster.
- The playing song is managed remotly so that it doesn't interfere with the game, using one function for playing the note that is passed as the duration interval finishes
