# String Operations

## Basics

You might've already seen that scalc has string support. If not, here is how you can define a string:

```
 > 'hello => "Hello" 
~> (=> 'hello "Hello")
=> "Hello"
```

Scalc also supports concatenation using `+` operator:

```
 > 'world => "World"
~> (=> 'world "World")
=> "World"

 > hello + 1 + world 
~> (+ (+ v/hello 1) v/world)
=> "Hello World"
```

As you can see, you can also add or subtract a number from a string. This either adds padding using spaces or trims the string:

```
 > "Hello, World!" - 8
~> (- "Hello, World!" 8)
=> "Hello"
```

You may as well use this feature to calculate string length:

```
 > 'strlen => (\ 'x ((x = "") . 0 (1 + (strlen ~ (x - 1)))))
~> (=> 'strlen (\ 'x (. (. (= v/x "") 0) (+ 1 (~ v/strlen (- v/x 1))))))
=> <lambda> (\ 'x (. (. (= v/x "") 0) (+ 1 (~ v/strlen (- v/x 1)))))

 > strlen ~ "Hello!"
~> (~ v/strlen "Hello!")
=> 6
```

## String multiplication

You may also multiply a string by an arbitrary integer just like this:

```
 > "Hello, World! " * 2
~> (* "Hello, World! " 2)
=> "Hello, World! Hello, World! "

 > "Hello, World!" * -1
~> (* "Hello, World!" -1)
=> "!dlroW ,olleH"

 > "Some string" * 0
~> (* "Some string" 0)
=> ""
```
