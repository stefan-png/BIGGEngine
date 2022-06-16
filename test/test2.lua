-- engine uses one single lua_State, so global objects are REALLY gloabl.

BIGGEngine.Callbacks.MouseButton = 100; -- warning: lua BIGGEngine.Callbacks.MouseButton is not a function!

function BIGGEngine.Callbacks.Init()
    print("[lua] calling BIGGEngine.Callbacks.Init in script "..BIGGEngine.ScriptName);
    mylib.subscribeMouseButton(6)
end
