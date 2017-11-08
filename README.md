Reprocessing example project
---

This is a starter example project. Just clone to get started!

## How to
```
git clone https://github.com/bsansouci/reprocessing-example.git
```

### Install
_warning_: this doesn't work with npm5 yet. Use npm4 or use yarn. To downgrade to npm4 you can `npm install -g npm@latest-4`
```
yarn
```

### Build
```
npm run build
```

This will build the bytecode executable which is at `./lib/bs/bytecode/index.byte`.

To build to JS run `npm run build:web` and then run a static server, like `python -m SimpleHTTPServer` and go to `localhost:8000`.

To build to native run `npm run build:native` and run `./lib/bs/native/index.native`

The build system used is [bsb-native](https://github.com/bsansouci/bsb-native).
