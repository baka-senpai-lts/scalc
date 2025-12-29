# scalc usage basics

## Running scalc

Now that you have scalc binary, you can invoke it like this:
```
$ ./scalc
 >
```

As for semantics in here:
- ` >` asks for your input
- `~>` gives you AST representation
- `=>` gives you result of your expression

You are now greeted with REPL. To begin, just start writing your expression. It will show you live AST representation so you can see what parser actually understands from it, feel free to experiment, here is an example session for you:
```
 > 2 + 2 * 2
~> (+ 2 (* 2 2))
=> 6

 > 'x => 10 
~> (=> 'x 10)
=> 10

 > x * 10
~> (* v/x 10)
=> 100

 > 'y ~> x
~> (~> 'y v/x)
=> <node> (@ v/x None)

 > y
~> (@ v/y None)
=> 10

 > 'x => 20
~> (=> 'x 20)
=> 20

 > y
~> (@ v/y None)
=> 20
```

## Operators

| Operator | Example                    | Description                                                                               |
|----------|----------------------------|-------------------------------------------------------------------------------------------|
| `+`      | `2 + 2`                    | Summarizes two values                                                                     |
| `-`      | `2 - 2`                    | Subtracts right value from left                                                           |
| `*`      | `2 * 2`                    | Multiplies two value                                                                      |
| `/`      | `2 / 2`                    | Divides left value by right, always returns a floating-point value                        |
| `\`      | `\ 'a (a + 1)`             | Creates a lambda function with last expression as a result                                |
| `~`      | `(\ 'a (a + 1)) ~ 2`       | Applies arguments to a lambda eagerly (will evaluate argument before substitution)        |
| `.`      | `(\ 'a (a + 1)) . (1 + 1)` | Applies arguments to a lambda lazily (will not evaluate before substitution)              |
| `=>`     | `'x => 10`                 | Binds variable to specific value, it will evaluate expression on the right before binding |
| `~>`     | `'y ~> x`                  | Binds variable to an expression on the right, that will be evaluated when called          |

## Name or value

Semantics are clear here, when you need a variable name to set variable or use it in lambda, prefix the name with `'`, this will be treated as a literal. Whenever you need to get a value that it holds, just type the name of the variable you need.

## Next steps

Now you can proceed to reading about [comparisons](./comparisons.md)
