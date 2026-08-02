## ribbit
Uses build system [ssteme](github.com/jangnathan/ssteme)
Except for wasm [here](https://jangnathan.github.io/ribbit/demo)

Supports these features:

``
print("hello world")
``

No else statements, so you must ``exit()`` early:

``
input = ask("enter password
(hint: tropical fruit with a crown, and bright yellow fleshy interior): ")

if input == "pineapple" {
    print("your in!")
    exit()
}

print("wrong password")
``

Out of the box Functions:
``
print("")
ask("")
str(123)
int("")
``

Declare variables:
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

It supports pemdas, but cannot parenthesize!
``
print(10 * 6 + 5 * 7 + 1 * 9 + 5)
``
test/
has features
