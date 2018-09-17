
--------------------------------------------------------------------------------------------

co = coroutine.create(function (a,b)
coroutine.yield(a + b, a - b)
print("coroutine test 01 coroutine yield back")
end)
print("coroutine test 01 -",coroutine.resume(co, 20, 10))  --> true 30 10

--------------------------------------------------------------------------------------------

co = coroutine.create (function ()
print("coroutine test 02 -", coroutine.yield())
end)
coroutine.resume(co)
coroutine.resume(co, 4, 5) --> co 4 5

--------------------------------------------------------------------------------------------

co = coroutine.create(function ()
return 6, 7
end)
print("coroutine test 03 -",coroutine.resume(co)) --> true 6 7

--------------------------------------------------------------------------------------------







