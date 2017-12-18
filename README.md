Reprocessing example project - 2048 Game
---
This was written as part of a livestream viewable here: https://www.youtube.com/watch?v=UDOEd5jS0Ac

<p align="center"> 
<img src="https://user-images.githubusercontent.com/4534692/32880874-172ea62e-ca64-11e7-9c12-c86df0019049.gif" />
</p>

## How to
```
git clone https://github.com/bsansouci/reprocessing-example.git
git checkout 2048
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
