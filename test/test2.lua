-- engine uses one single lua_State, so global objects are REALLY gloabl.

function BIGGEngine.Callbacks.Init()
    print("[lua] calling BIGGEngine.Callbacks.Init!");
    mylib.subscribe(6)
end


