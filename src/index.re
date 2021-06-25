open Reprocessing;

type state = {
  stars: list((float, float)),
  path: list((float, float)),
  winningArea: float,
  gameover: bool,
  youWon: bool,
};

let generateInitiatPath = () => {
  let initialPosition = (0., Utils.randomf(0., 600.));
  [initialPosition, initialPosition];
};

let winningHeight = 100.;

let setup = env => {
  Env.size(~width=600, ~height=600, env);
  let l = ref([]);
  for (i in 0 to 99) {
    let running = ref(true);
    while (running^) {
      let (nx, ny) = (Utils.randomf(0., 600.), Utils.randomf(0., 600.));
      let farFromEverythingElse =
        List.for_all(
          ((sx, sy)) => {
            let dx = nx -. sx;
            let dy = ny -. sy;
            let dist = sqrt(dx *. dx +. dy *. dy);
            dist > 50.;
          },
          l^,
        );
      if (farFromEverythingElse) {
        l := [(nx, ny), ...l^];
        running := false;
      };
    };
  };
  {
    stars: l^,
    path: generateInitiatPath(),
    gameover: false,
    youWon: false,
    winningArea: Utils.randomf(0., 600. -. winningHeight),
  };
};

let drawGameOverScreen = (state, color, env) => {
  Draw.noStroke(env);
  Draw.fill(Utils.color(0, 0, 0, 100), env);
  Draw.rect(~pos=(0, 0), ~width=600, ~height=600, env);
  let body = "Game Over";
  let textWidth = Draw.textWidth(~body, env);
  let padding = 13;
  Draw.stroke(color, env);
  Draw.strokeWeight(10, env);
  Draw.strokeCap(Round, env);
  Draw.fill(color, env);
  let y = 150;
  Draw.rect(
    ~pos=(300 - textWidth / 2 - padding, y - padding),
    ~width=textWidth + padding * 2,
    ~height=54,
    env,
  );
  Draw.text(~pos=(300 - textWidth / 2, y), ~body, env);
};

let drawYouWonScreen = (state, color, env) => {
  Draw.noStroke(env);
  Draw.fill(Utils.color(0, 0, 0, 100), env);
  Draw.rect(~pos=(0, 0), ~width=600, ~height=600, env);
  let body = "You Won!";
  let textWidth = Draw.textWidth(~body, env);
  let padding = 13;
  Draw.stroke(color, env);
  Draw.strokeWeight(10, env);
  Draw.strokeCap(Round, env);
  Draw.fill(color, env);
  let y = 150;
  Draw.rect(
    ~pos=(300 - textWidth / 2 - padding, y - padding),
    ~width=textWidth + padding * 2,
    ~height=54,
    env,
  );
  Draw.text(~pos=(300 - textWidth / 2, y), ~body, env);
};

let starRadius = 10.;

let draw = (state, env) => {
  Draw.background(Utils.color(~r=100, ~g=100, ~b=100, ~a=255), env);
  let green = Utils.color(~r=176, ~g=215, ~b=179, ~a=255);
  Draw.fill(green, env);

  let (x, y) = List.hd(state.path);
  let (nx, ny) = (x +. 1., Env.key(Space, env) ? y +. 1. : y -. 1.);
  let state =
    if (!state.gameover) {
      let collidesWithStar =
        List.exists(
          ((sx, sy)) => {
            let dx = nx -. sx;
            let dy = ny -. sy;
            let dist = sqrt(dx *. dx +. dy *. dy);
            dist < starRadius;
          },
          state.stars,
        );

      let state =
        if (collidesWithStar) {
          {...state, gameover: true};
        } else {
          {...state, path: [(nx, ny), ...List.tl(state.path)]};
        };
      let state =
        if (Env.keyPressed(Space, env) || Env.keyReleased(Space, env)) {
          {...state, path: [List.hd(state.path), ...state.path]};
        } else {
          state;
        };
      state;
    } else {
      let state =
        if (Env.keyPressed(Space, env)) {
          {...state, gameover: false, path: generateInitiatPath()};
        } else {
          state;
        };
      state;
    };

  let state =
    if (nx > 600.
        && ny > state.winningArea
        && ny < state.winningArea
        +. winningHeight) {
      {...state, youWon: true};
    } else {
      state;
    };

  Draw.noStroke(env);
  List.iter(
    pos =>
      Draw.ellipsef(~center=pos, ~radx=starRadius, ~rady=starRadius, env),
    state.stars,
  );

  Draw.stroke(green, env);
  Draw.strokeWeight(2, env);
  let rec drawPath = path => {
    switch (path) {
    | [] => ()
    | [_] => ()
    | [first, second, ...rest] =>
      Draw.linef(first, second, env);
      drawPath([second, ...rest]);
    };
  };
  drawPath(state.path);

  Draw.strokeWeight(10, env);
  Draw.linef(
    (600., state.winningArea),
    (600., state.winningArea +. winningHeight),
    env,
  );

  if (state.gameover) {
    drawGameOverScreen(state, green, env);
  };

  if (state.youWon) {
    drawYouWonScreen(state, green, env);
  };

  state;
};

run(~setup, ~draw, ());
