## ribbit
This is a interpreted scripting language using AST
Built with C and uses a custom build system called [ssteme](github.com/jangnathan/ssteme)
apart from the wasm demo [here](https://jangnathan.github.io/ribbit/demo)

currently supports these features:

``
print("hello world")
``

it does not have else statements, so you must ``exit()`` early:

``
input = ask("enter password
(hint: it is the name of a tropical fruit with a crown, and bright yellow flesh): ")

if input == "pineapple" {
    print("your in!")
    exit()
}

print("wrong password")
``

here are the functions you can use:
``
print("")
ask("")
str(123)
int("")
``

Heres how to declare variables:
``
my_var = "hello world"
print(my_var)
``

Heres how to declare loops:
``
for i = 0, i < 10, i = i + 1 {
print(str(i))
}
``

It supports pemdas (without the parenthesis)!
``
print(10 * 6 + 5 * 7 + 1 * 9 + 5)
``

it is buggy because there is no lexer

it **does not have commenting**, and also no lexer, so it will be very difficult to scale this language
A bytecode implementation would be more practical and lighter on memory

look in test/
for features
