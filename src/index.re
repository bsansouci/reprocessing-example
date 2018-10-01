open Reprocessing;

let setup = (env) => Env.size(~width=600, ~height=600, env);

let draw = (_state, env) => {
  Draw.background(Utils.color(~r=255, ~g=217, ~b=229, ~a=255), env);
  Draw.fill(Utils.color(~r=41, ~g=166, ~b=244, ~a=255), env);
  Draw.rect(~pos=(150, 150), ~width=300, ~height=300, env)
};

run(~setup, ~draw, ());
