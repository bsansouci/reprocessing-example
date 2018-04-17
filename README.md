Flappy Bird Livestream
---

This is a starter example project. Just clone to get started!

This was written in 3 hours as part of a livestream which can be viewed here: https://www.youtube.com/watch?v=5aD3aPvNpyQ

<p align="center"> 
<img src="https://user-images.githubusercontent.com/4534692/34076455-e047b276-e29c-11e7-9f14-33f79b0d4612.gif" />
</p>

## How to
```
git clone https://github.com/bsansouci/reprocessing-example.git
git checkout livestream-flappybird
```
### Install
```
npm install
```

### Build
```
npm run build
```

### Start
```
npm start
```

This will build the bytecode executable which is at `./lib/bs/bytecode/index.byte`.

To build to JS run `npm run build:web` and then run a static server, like `python -m SimpleHTTPServer` and go to `localhost:8000`.

To build to native run `npm run build:native` and run `./lib/bs/native/index.native`

The build system used is [bsb-native](https://github.com/bsansouci/bsb-native).
