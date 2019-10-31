# Laserage bot
A bot for Windows that can win the game LaserAge. LaserAge is a 2D space shooter from 1999 which I used to play during my childhood. I decided to make a bot because I've never been able to beat it manually. I've made a Youtube video winning the game for the first time, while also adding the final touches to the code, [link here](https://www.youtube.com/watch?v=UdI1vJwmLKI).

![Game Screenshot](sample_graphics/game_screenshot.jpg)

You can download the game from [here](http://www.freegamesarea.com/laserage-gold.html) (I'm not responsible for that website).

# How it works
The bot works by reading the screen and avoiding the bullets by controlling the computer mouse. It calculates which positions don't have bullets in an area as large as the ship and moves to one of those randomly every 10ms. The bullets are detected by color, and the bot logic works in linear time by scanning the screen in a [prefix sum](https://en.wikipedia.org/wiki/Prefix_sum) fashion. It also detects power ups and tries to catch them. The ship fires at random.

# How to use
Run the bot and start the game. Make sure the game window is active. Then, press F6 to start / pause the bot.

Note: You can get the already compiled bot in the release section.

# Compilation
> cmake .
