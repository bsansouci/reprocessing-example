Flappybird
---

This project is made using [Reprocessing](https://github.com/schmavery/reprocessing).

This project builds using the build system [bsb-native](https://github.com/bsansouci/bsb-native) which allows the project to be built to **JS** as well as built to **bytecode** and **native assembly**.

<p align="center"> 
<img src="https://user-images.githubusercontent.com/4534692/34076455-e047b276-e29c-11e7-9f14-33f79b0d4612.gif" />
</p>


## How to
```
git clone https://github.com/bsansouci/reprocessing-example.git
```

### Install
_warning_: this doesn't work with npm5 yet. Use npm4 or use yarn. To downgrade to npm4 you can `npm install -g npm@latest-4`
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

To build to JS run `npm run build:web` and then run a static server, like `python -m SimpleHTTPServer` and go to `localhost:8000`. If you're using safari you can simply open the `index.html` and tick `Develop > Disable Cross-Origin Restrictions`.

To build to native run `npm run build:native` and run `npm run start:native`

The build system used is [bsb-native](https://github.com/bsansouci/bsb-native).
