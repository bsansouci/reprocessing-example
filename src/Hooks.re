type animationT = {
  running: bool,
  time: float,
  value: float,
  velocity: float,
  stiffness: float,
  damping: float,
  destination: float,
  lastFrameValue: float,
  lastFrameVelocity: float,
};

/* Good enough */
/*noWobble: { stiffness: 170, damping: 26 }, // the default, if nothing provided
  gentle: { stiffness: 120, damping: 14 },
  wobbly: { stiffness: 180, damping: 12 },
  stiff: { stiffness: 210, damping: 20 },*/
let stepper =
    (
      realdt: float,
      x: float,
      v: float,
      destX: float,
      stiffness: float,
      damping: float,
    ) => {
  /*precision: float,*/

  let precision = 0.5;

  /* Spring stiffness, in kg / s^2 */

  /* for animations, destX is really spring length (spring at rest). initial */
  /* position is considered as the stretched/compressed position of a spring */
  let fspring = -. stiffness *. (x -. destX);

  /* Damping, in kg / s */
  let fdamper = -. damping *. v;

  /* usually we put mass here, but for animation purposes, specifying mass is a */
  /* bit redundant. you could simply adjust stiffness and damping accordingly */
  /* let a = (fspring + fdamper) / mass; */
  let a = fspring +. fdamper;

  let newV = v +. a *. realdt;
  let newX = x +. newV *. realdt;

  if (abs_float(newV) < precision && abs_float(newX -. destX) < precision) {
    (destX, 0.);
  } else {
    (newX, newV);
  };
};

type t = Hashtbl.t(string, string);

let initialState: t = Hashtbl.create(10);

let useState = (stateMap, initialValue, ident) => {
  let setState = (v) => {
    Hashtbl.replace(stateMap, ident, Obj.magic(v));
  };
  switch (Hashtbl.find(stateMap, ident)) {
    | exception Not_found =>
      Hashtbl.add(stateMap, ident, Obj.magic(initialValue));
      (Obj.magic(initialValue), setState)
    | s => (Obj.magic(s), setState)
  };
};
let msPerFrame = 1000. /. 60.;
let useSpring = (stateMap, dt, ~initialValue=0., ~initialVelocity=0., ~destination, ~stiffness, ~damping, ident) : (animationT, unit => unit, (animationT) => unit) => {
  let initialSpring = {
    time: 0.,
    value: initialValue,
    velocity: initialVelocity,
    stiffness,
    damping,
    destination,
    running: false,
    lastFrameValue: initialValue,
    lastFrameVelocity: initialVelocity
  };
  let (spring, setSpring) = useState(stateMap, initialSpring, ident);
  let resetSpring = () => {
    setSpring(initialSpring);
  };
  let time = spring.time +. (dt *. 1000.);
  let currentFrameCompletion =
    (time -.
      floor(time /. msPerFrame) *. msPerFrame) /.
    msPerFrame;
  let framesToCatchUp = int_of_float(time /. msPerFrame);

  let newValue = ref(spring.lastFrameValue);
  let newVelocity = ref(spring.lastFrameVelocity);
  for (i in 0 to framesToCatchUp - 1) {
    let (value, velocity) =
      stepper(
        msPerFrame /. 1000.,
        newValue^,
        newVelocity^,
        spring.destination,
        spring.stiffness,
        spring.damping,
      );
    newValue := value;
    newVelocity := velocity;
  };
  let (nextFrameValue, nextFrameVelocity) =
      stepper(
        msPerFrame /. 1000.,
        newValue^,
        newVelocity^,
        spring.destination,
        spring.stiffness,
        spring.damping,
      );
  let newSpring = {
    ...spring,
    value: newValue^ +.
        (nextFrameValue -. newValue^) *. currentFrameCompletion,
    velocity: newVelocity^ +.
        (nextFrameVelocity -. newVelocity^) *. currentFrameCompletion,
    lastFrameValue: newValue^,
    lastFrameVelocity: newVelocity^,
    time : time -. float_of_int(framesToCatchUp) *. msPerFrame
  };

  setSpring(newSpring);
  (spring, resetSpring, setSpring);
};

let useTimer = (stateMap, dt, ident) => {
  let (timer, setTimer) = useState(stateMap, 0., ident);
  let resetTimer = () => setTimer(0.);

  let newTimer = timer +. dt;
  setTimer(newTimer);
  (timer, resetTimer, setTimer);
};
