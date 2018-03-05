open Reprocessing;

let blockSize = 100;

let padding = 20;

let paddingAround = 20;

let puzzleSize = 4;

let maxAnimationTime = 0.1;

let maxAnimationCombiningTime = 0.1;

let backgroundColor = Utils.color(~r=184, ~g=173, ~b=162, ~a=255);

let allColors = [|
  Utils.color(~r=203, ~g=193, ~b=181, ~a=255),
  Utils.color(~r=236, ~g=228, ~b=219, ~a=255),
  Utils.color(~r=235, ~g=225, ~b=203, ~a=255),
  Utils.color(~r=232, ~g=180, ~b=130, ~a=255),
  Utils.color(~r=227, ~g=153, ~b=103, ~a=255),
  Utils.color(~r=223, ~g=129, ~b=101, ~a=255), /* 32 */
  Utils.color(~r=246, ~g=94, ~b=59, ~a=255),
  Utils.color(~r=237, ~g=207, ~b=114, ~a=255),
  Utils.color(~r=237, ~g=204, ~b=97, ~a=255),
  Utils.color(~r=237, ~g=200, ~b=80, ~a=255),
  Utils.color(~r=237, ~g=197, ~b=63, ~a=255),
  Utils.color(~r=237, ~g=194, ~b=46, ~a=255),
  Utils.color(~r=60, ~g=58, ~b=50, ~a=255)
|];

type destinationT = {
  x: int,
  y: int
};

type animationT =
  | NotAnimating
  | Moving(destinationT)
  | Combining;

type gridPieceT = {
  number: int,
  animation: animationT
};

type globalAnimationT =
  | NotAnimatingGlobal
  | MovingGlobal(float)
  | CombiningGlobal(float);

type stateT = {
  grid: array(array(gridPieceT)),
  font: fontT,
  fontSmall: fontT,
  score: int,
  animationTime: globalAnimationT,
  running: bool,
  mousePressed: bool
};

let clone = arr => Array.map(Array.copy, arr);

let emptyCell = {animation: NotAnimating, number: 0};

let addNewElement = ({grid} as state) => {
  let (emptyBlocks, _, _) =
    Array.fold_left(
      ((emptyBlocks, _, y), row) =>
        Array.fold_left(
          ((emptyBlocks, x, y), {number}) =>
            if (number === 0) {
              ([(x, y), ...emptyBlocks], x + 1, y);
            } else {
              (emptyBlocks, x + 1, y);
            },
          (emptyBlocks, 0, y + 1),
          row
        ),
      ([], 0, (-1)),
      grid
    );
  if (List.length(emptyBlocks) === 0) {
    failwith("This should not happen");
  };
  let (x, y) = List.nth(emptyBlocks, Random.int(List.length(emptyBlocks)));
  let newGrid = clone(grid);
  let number = Random.int(10) === 1 ? 2 : 1;
  newGrid[y][x] = {animation: Combining, number};
  {...state, grid: newGrid};
};

let groupBy2 = row => {
  let groups =
    Array.fold_right(
      ({number}, acc) =>
        if (number === 0) {
          acc;
        } else {
          switch acc {
          | [[previous], ...rest] when previous === number => [
              [previous, number],
              ...rest
            ]
          | _ => [[number], ...acc]
          };
        },
      row,
      []
    );
  let size = List.length(groups);
  let rec recur = (acc, i, max) =>
    if (i >= max) {
      acc;
    } else {
      recur([[], ...acc], i + 1, max);
    };
  recur(groups, 0, puzzleSize - size);
};

let movePieces = ({grid} as state) => {
  let isAnythingAnimating = ref(false);
  let grid =
    Array.mapi(
      (y, row) => {
        let groups = groupBy2(row);
        /*let i = ref(0);*/
        let j = ref(0);
        let alreadySeenThis = ref(false);
        let newRow = Array.make(puzzleSize, emptyCell);
        for (i in 0 to puzzleSize - 1) {
          if (row[i].number > 0) {
            while (List.length(List.nth(groups, j^)) === 0) {
              j := j^ + 1;
            };
            let subList = List.nth(groups, j^);
            assert (row[i].number === List.nth(subList, 0));
            let animation =
              if (j^ === i) {
                NotAnimating;
              } else {
                isAnythingAnimating := true;
                Moving({x: j^, y});
              };
            newRow[i] = {...row[i], animation};
            if (List.length(subList) === 2 && ! alreadySeenThis^) {
              alreadySeenThis := true;
            } else {
              alreadySeenThis := false;
              j := j^ + 1;
            };
          };
        };
        newRow;
      },
      grid
    );
  {
    ...state,
    animationTime: isAnythingAnimating^ ? MovingGlobal(0.) : NotAnimatingGlobal,
    grid
  };
};

let superMegaRotationStuff = ({grid} as state) => {
  let newGrid = Array.make_matrix(puzzleSize, puzzleSize, emptyCell);
  for (y in 0 to puzzleSize - 1) {
    for (x in 0 to puzzleSize - 1) {
      let newCell = grid[puzzleSize - 1 - x][y];
      let animation =
        switch grid[puzzleSize - 1 - x][y].animation {
        | Moving({x, y}) => Moving({x: puzzleSize - 1 - y, y: x})
        | a => a
        };
      newGrid[y][x] = {...newCell, animation};
    };
  };
  {...state, grid: newGrid};
};

let moveUp = state =>
  state
  |> superMegaRotationStuff
  |> movePieces
  |> superMegaRotationStuff
  |> superMegaRotationStuff
  |> superMegaRotationStuff;

let moveDown = state =>
  state
  |> superMegaRotationStuff
  |> superMegaRotationStuff
  |> superMegaRotationStuff
  |> movePieces
  |> superMegaRotationStuff;

let moveLeft = state =>
  state
  |> superMegaRotationStuff
  |> superMegaRotationStuff
  |> movePieces
  |> superMegaRotationStuff
  |> superMegaRotationStuff;

let moveRight = state => movePieces(state);

let compareGrid = (grid1, grid2) => {
  let rec loop = (x, y) =>
    if (y >= puzzleSize) {
      true;
    } else if (x >= puzzleSize) {
      loop(0, y + 1);
    } else if (grid1[y][x].number !== grid2[y][x].number) {
      false;
    } else {
      loop(x + 1, y);
    };
  loop(0, 0);
};

let resolveHelper = ({grid, score} as state) => {
  let newGrid = Array.make_matrix(puzzleSize, puzzleSize, emptyCell);
  let score = ref(score);
  for (y in 0 to puzzleSize - 1) {
    for (x in 0 to puzzleSize - 1) {
      switch grid[y][x] {
      | {animation: Moving({x: x2, y: y2}), number} =>
        if (newGrid[y2][x2].number > 0) {
          score := score^ + Utils.pow(~base=2, ~exp=number + 1);
          newGrid[y2][x2] = {animation: Combining, number: number + 1};
        } else {
          newGrid[y2][x2] = {animation: NotAnimating, number};
        }
      | {number: 0} => ()
      | cell =>
        if (newGrid[y][x].number > 0) {
          score := score^ + Utils.pow(~base=2, ~exp=cell.number + 1);
          newGrid[y][x] = {animation: Combining, number: cell.number + 1};
        } else {
          newGrid[y][x] = cell;
        }
      };
    };
  };
  {...state, grid: newGrid, score: score^};
};

let resolveCombining = ({grid} as state) => {
  let newGrid = Array.make_matrix(puzzleSize, puzzleSize, emptyCell);
  for (y in 0 to puzzleSize - 1) {
    for (x in 0 to puzzleSize - 1) {
      switch grid[y][x] {
      | {animation: Combining(_), number} =>
        newGrid[y][x] = {animation: NotAnimating, number}
      | cell => newGrid[y][x] = cell
      };
    };
  };
  {...state, grid: newGrid};
};

let resolveMove = state => {
  let state = addNewElement(resolveHelper(state));
  if (compareGrid(state.grid, resolveHelper(moveLeft(state)).grid)
      && compareGrid(state.grid, resolveHelper(moveRight(state)).grid)
      && compareGrid(state.grid, resolveHelper(moveUp(state)).grid)
      && compareGrid(state.grid, resolveHelper(moveDown(state)).grid)) {
    {...state, running: false};
  } else {
    state;
  };
};

let drawCell = (x, y, n, state, env) => {
  Draw.fill(allColors[n], env);
  Draw.stroke(allColors[n], env);
  Draw.strokeWeight(5, env);
  Draw.rect(~pos=(x, y), ~width=blockSize, ~height=blockSize, env);
  switch n {
  | 0 => ()
  | 1
  | 2
  | 3 =>
    Draw.text(
      ~font=state.font,
      ~body=string_of_int(Utils.pow(~base=2, ~exp=n)),
      ~pos=(x + 28, y + 75),
      env
    )
  | 4
  | 5
  | 6 =>
    Draw.text(
      ~font=state.font,
      ~body=string_of_int(Utils.pow(~base=2, ~exp=n)),
      ~pos=(x + 6, y + 75),
      env
    )
  | _ =>
    Draw.text(
      ~font=state.fontSmall,
      ~body=string_of_int(Utils.pow(~base=2, ~exp=n)),
      ~pos=(x + 17, y + 75),
      env
    )
  };
};

let setup = env => {
  Env.size(~width=600, ~height=700, env);
  addNewElement({
    grid: Array.make_matrix(puzzleSize, puzzleSize, emptyCell),
    score: 0,
    font: Draw.loadFont(~filename="assets/font_2x.fnt", env),
    fontSmall: Draw.loadFont(~filename="assets/fontSmall_2x.fnt", env),
    animationTime: CombiningGlobal(0.),
    running: true,
    mousePressed: false
  });
};

let draw = (state, env) => {
  Draw.background(Utils.color(~r=199, ~g=217, ~b=229, ~a=255), env);
  Draw.fill(backgroundColor, env);
  let puzzleSizePx = (blockSize + padding) * puzzleSize - padding;
  let xOffset = (Env.width(env) - puzzleSizePx) / 2;
  let yOffset = (Env.height(env) - puzzleSizePx) / 2 + 50;
  Draw.stroke(backgroundColor, env);
  Draw.rect(
    ~pos=(xOffset - paddingAround, yOffset - paddingAround),
    ~width=puzzleSizePx + paddingAround * 2,
    ~height=puzzleSizePx + paddingAround * 2,
    env
  );
  Draw.text(
    ~font=state.font,
    ~body="Score: " ++ string_of_int(state.score),
    ~pos=(xOffset - paddingAround, 95),
    env
  );
  let rectX = xOffset + puzzleSizePx - 110;
  let rectY = 95;
  let rectWidth = 130;
  let rectHeight = 40;
  Draw.rect(~pos=(rectX, rectY), ~width=rectWidth, ~height=rectHeight, env);
  Draw.text(
    ~font=state.fontSmall,
    ~body="Restart",
    ~pos=(xOffset + puzzleSizePx - 100, 128),
    env
  );
  let (mx, my) = Env.mouse(env);
  let state =
    if (state.mousePressed
        && mx < rectX
        + rectWidth
        && mx > rectX
        && my < rectY
        + rectHeight
        && my > rectY) {
      addNewElement({
        grid: Array.make_matrix(puzzleSize, puzzleSize, emptyCell),
        score: 0,
        font: Draw.loadFont(~filename="assets/font_2x.fnt", env),
        fontSmall: Draw.loadFont(~filename="assets/fontSmall_2x.fnt", env),
        animationTime: CombiningGlobal(0.),
        running: true,
        mousePressed: false
      });
    } else {
      state;
    };
  /*  let state =
      if (Env.keyPressed(Space, env)) {
        {...state, grid: addNewElement(state.grid)}
      } else {
        state
      };*/
  let state =
    if (Env.keyPressed(A, env)) {
      superMegaRotationStuff(state);
    } else {
      state;
    };
  let state =
    if (Env.keyPressed(Right, env)) {
      let state =
        switch state.animationTime {
        | NotAnimatingGlobal => state
        | CombiningGlobal(_) => resolveCombining(state)
        | MovingGlobal(_) => resolveMove(state)
        };
      moveRight(state);
    } else {
      state;
    };
  let state =
    if (Env.keyPressed(Up, env)) {
      let state =
        switch state.animationTime {
        | NotAnimatingGlobal => state
        | CombiningGlobal(_) => resolveCombining(state)
        | MovingGlobal(_) => resolveMove(state)
        };
      moveUp(state);
    } else {
      state;
    };
  let state =
    if (Env.keyPressed(Left, env)) {
      let state =
        switch state.animationTime {
        | NotAnimatingGlobal => state
        | CombiningGlobal(_) => resolveCombining(state)
        | MovingGlobal(_) => resolveMove(state)
        };
      moveLeft(state);
    } else {
      state;
    };
  let state =
    if (Env.keyPressed(Down, env)) {
      let state =
        switch state.animationTime {
        | NotAnimatingGlobal => state
        | CombiningGlobal(_) => resolveCombining(state)
        | MovingGlobal(_) => resolveMove(state)
        };
      moveDown(state);
    } else {
      state;
    };
  let state =
    switch state.animationTime {
    | NotAnimatingGlobal => state
    | MovingGlobal(animationTime) when animationTime >= maxAnimationTime =>
      resolveMove({...state, animationTime: CombiningGlobal(0.)})
    | MovingGlobal(animationTime) => {
        ...state,
        animationTime:
          MovingGlobal(
            min(animationTime +. Env.deltaTime(env), maxAnimationTime)
          )
      }
    | CombiningGlobal(animationTime)
        when animationTime >= maxAnimationCombiningTime =>
      resolveCombining({...state, animationTime: NotAnimatingGlobal})
    | CombiningGlobal(animationTime) => {
        ...state,
        animationTime:
          CombiningGlobal(
            min(animationTime +. Env.deltaTime(env), maxAnimationCombiningTime)
          )
      }
    };
  Array.iteri(
    (y, row) =>
      Array.iteri(
        (x, _) => {
          let x = xOffset + x * (blockSize + padding);
          let y = yOffset + y * (padding + blockSize);
          drawCell(x, y, 0, state, env);
        },
        row
      ),
    state.grid
  );
  Array.iteri(
    (y, row) =>
      Array.iteri(
        (x, {number, animation}) => {
          let x = xOffset + x * (blockSize + padding);
          let y = yOffset + y * (padding + blockSize);
          if (number > 0) {
            switch animation {
            | Combining =>
              let value =
                switch state.animationTime {
                | CombiningGlobal(animationTime) =>
                  animationTime /. maxAnimationCombiningTime
                | MovingGlobal(_)
                | NotAnimatingGlobal => assert false
                };
              Draw.pushMatrix(env);
              Draw.translate(
                float_of_int(x + blockSize / 2),
                float_of_int(y + blockSize / 2),
                env
              );
              Draw.scale(
                Utils.lerpf(0.2, 1.1, value),
                Utils.lerpf(0.2, 1.1, value),
                env
              );
              Draw.translate(
                float_of_int(- blockSize / 2),
                float_of_int(- blockSize / 2),
                env
              );
              drawCell(0, 0, number, state, env);
              Draw.popMatrix(env);
            | NotAnimating => drawCell(x, y, number, state, env)
            | Moving({x: destX, y: destY}) =>
              let endX = xOffset + destX * (blockSize + padding);
              let endY = yOffset + destY * (padding + blockSize);
              let value =
                switch state.animationTime {
                | CombiningGlobal(_)
                | NotAnimatingGlobal => assert false
                | MovingGlobal(animationTime) =>
                  animationTime /. maxAnimationTime
                };
              drawCell(
                Utils.lerp(x, endX, value),
                Utils.lerp(y, endY, value),
                number,
                state,
                env
              );
            };
          };
        },
        row
      ),
    state.grid
  );
  if (! state.running) {
    Draw.fill(backgroundColor, env);
    Draw.stroke(backgroundColor, env);
    Draw.rect(~pos=(xOffset + 30, 330), ~width=390, ~height=90, env);
    Draw.text(
      ~font=state.font,
      ~body="Game Over",
      ~pos=(xOffset + 50, 400),
      env
    );
  };
  {...state, grid: state.grid, mousePressed: false};
};

let mouseDown = (state, env) => {...state, mousePressed: true};

run(~setup, ~draw, ~mouseDown, ());
