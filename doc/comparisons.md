# Comparisons

## How to know if one number is bigger than the other?

scalc uses conventional comparison operators, however please note that syntax like `1 + 1 > 1` isn't supported and isn't planned yet, so correct way to write this would be `(1 + 1) > 1`.

Comparisons in scalc return a function, be not afraid, these are Church-encoded booleans. Here are examples for `true` and `false` written manually in case you need them. Lambdas returned by comparisons will mimic the structure:

```
 > 'true => (\ 'x 'y (x)) 
~> (=> 'true (\ 'x (\ 'y (@ v/x None))))
=> <lambda> (\ 'x (\ 'y (@ v/x None)))

 > 'false => (\ 'x 'y (y)) 
~> (=> 'false (\ 'x (\ 'y (@ v/y None))))
=> <lambda> (\ 'x (\ 'y (@ v/y None)))
```

As you can see, `true` returns left value and `false` returns right value.

## How to use these booleans

The major advantage of these booleans is that you don't need a dedicated `if` to use them. So if you want to return `n` only if it's greater than 5 and otherwise return 5, here is how it will look:

```
 > 'n => 10 
~> (=> 'n 10)
=> 10

 > (n > 5) . n 5  
~> (. (. (> v/n 5) v/n) 5)
=> 10

 > 'n => 3
~> (=> 'n 3)
=> 3

 > (n > 5) . n 5 
~> (. (. (> v/n 5) v/n) 5)
=> 5
```

As you can see, there is no `if` statement, only application to a function returned by a comparison operation. This will work absolutely the same for every other comparison operator. But if you really want to, here is how you would write an `if` function:

```
'if => (\ 'cond 'then 'else (cond . then else))
```

## Side-effects

When using a boolean you often don't want to evaluate a redundant node. That's why you are encouraged to use lazy application for them instead of eager evaluation. So just take it as a rule to not use eager evaluation with booleans.

## Example

Here is an example factorial function that uses comparisons for control flow:

```
 > 'factorial => (\ 'n ((n > 1) . (n * (factorial ~ (n - 1))) 1))
~> (=> 'factorial (\ 'n (. (. (> v/n 1) (* v/n (~ v/factorial (- v/n 1)))) 1)))
=> <lambda> (\ 'n (. (. (> v/n 1) (* v/n (~ v/factorial (- v/n 1)))) 1))

 > factorial ~ 5
~> (~ v/factorial 5)
=> 120
```

You can use the same pattern to write anything you want.

## All operators

| Operator | Example    |
|----------|------------|
| `=`      | `2 = 2`    |
| `!=`     | `2 != 1`   |
| `>`      | `2 > 1`    |
| `<`      | `2 < 3`    |
| `>=`     | `2 >= 1.5` |
| `<=`     | `2 <= 2`   |
