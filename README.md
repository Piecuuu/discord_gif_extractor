# Discord Gif Extractor
## What is it?
Well, it's basically a little program to extract gif links from your gif favorites.
## Can it access my account?
No, it can't. Every action that you do on your account while setting up the program is done purely by you. The program also can't access any of your personal info, tokens and stuff like that.*

<sub>* While using the `--token` parameter the app can access the account you have provided the token to. It will only get the frecency user data (GIF favorites and most used/favorite emojis). (`https://discord.com/api/v9/users/@me/settings-proto/2`)</sub>
## Usage
### Downloading the source code
```sh
git clone --recursive https://github.com/Piecuuu/discord_gif_extractor
cd discord_gif_extractor
```
### Building
```sh
mkdir build &&
cd build
cmake ..
make
```
### Using
#### Getting the encoded data
TODO: Write about manually getting it. The newest update includes `-t` with usage is described in [Option A](#option-a-providing-your-discord-token-for-the-program-to-automatically-pull-in-the-data) or usage.

#### Using the extractor

#### Option A: Providing your Discord Token for the program to automatically pull in the data.
It's as easy as running (replace `TOKEN` with your actual token):
```sh
./discord_gif_extractor -t TOKEN
```

#### Option B: Decode data you got by manually sending a GET request or by looking in the Network tab of devtools.
Go to the directory you've built the program in (it's probably `build/`).

Create a file named `data.txt` and paste the encoded data inside the file.

Now run the extractor:
```sh
./discord_gif_extractor
```

A file named `links.txt` should appear in the current directory.
