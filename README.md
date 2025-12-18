# scalc - Suddenly Complete cALCulator

I built this calculator for personal usage and added lambdas. Now it's Turing-complete.

## About project

This is a fully functioning calculator, that happens to also be fully programmable. You can write a function as a lambda. If you are familiar with lambda calculus, this piece of code will represent calling an omega combinator:

``` text
~ (\ 'x (~ x x)) (\ 'x (~ x x))
```

But be aware that this WILL crash scalc (unless I added stack overflow protection and forgot to edit this line). Anyway, enjoy chaos.

## Compilation

The only dependencies it has:
- gcc
- make
- libc

Once you have all of these, you can proceed to compile it with `make`. Full path is as follows:

``` sh
git clone https://gitlab.com/baka.senpai.lts/scalc.git
cd scalc
make
```

## Usage

Just run `./scalc` and try to write something. Here is an example of what it absolutely can do:

``` text
 > 2 + 2 * 2
~> (+ 2 (* 2 2))
=> 6

 > + 1 2 3 4 5
~> (+ (+ (+ (+ 1 2) 3) 4) 5)
=> 15

 > ~ (\ 'x (* x x x)) 5
~> (~ (\ 'x (* (* v/x v/x) v/x)) 5)
=> 125

 > 19 / 2 - 10
~> (- (/ 19 2) 10)
=> -0.500000

 > ~ (\ 'f 'a (~ f (~ f a))) (\ 'x (* x x)) 5
~> (~ (~ (\ 'f (\ 'a (~ v/f (~ v/f v/a)))) (\ 'x (* v/x v/x))) 5)
=> 625
```

You also can write it in postfix like `2 2 +`, however it's currently not clearly defined what will happen if you add third operand like `2 2 2 +`. So do not rely on it's behavior too much.

As for output, `~>` in REPL is here to show you what it figured out from your string, `=>` is for result of your expression.

## Testing

To check code for memory leaks automatically, try to run `./run_random.sh`, it will feed scalc with 1000 random chunks, each containing 800 bytes. If it segfaults, check if it accidentally created an omega combinator, because you might be witnessing a divine intervention. If it's not anything like this and it just leaks memory, please, open an issue. For manual testing just run it with valgrind, it works fairly well for this project.

## Fun facts

- I checked for everything I could and didn't find a way it could possibly leak memory without crashing.
- Binary size is currently 24KB on my machine with `-Oz`.
- Parser is so permissive that it won't ever throw you an error. But if I myself couldn't make sense of whatever you wrote, parser won't too.

## Known issues

- It segfaults on infinite recursion.
- It might be painfully slow in some cases, but I didn't find them.
