BIGGEngine = {}

-- Stuff which changes per script
BIGGEngine.ScriptName = "NullScript"    -- String containing debug name of script. Set by C API.
BIGGEngine.Callbacks = {}               -- Table of callbacks which is saved per script. Set by script.

-- Stuff which changes per component
BIGGEngine.Persistent = {}              -- Table of persistent data which is saved per component. Set by script.

BIGGEngine.MouseButtonEnum = {}
BIGGEngine.MouseButtonEnum.Zero    = 0
BIGGEngine.MouseButtonEnum.One     = 1
BIGGEngine.MouseButtonEnum.Two     = 2
BIGGEngine.MouseButtonEnum.Three   = 3
BIGGEngine.MouseButtonEnum.Four    = 4
BIGGEngine.MouseButtonEnum.Five    = 5
BIGGEngine.MouseButtonEnum.Six     = 6
BIGGEngine.MouseButtonEnum.Seven   = 7
BIGGEngine.MouseButtonEnum.Left    = BIGGEngine.MouseButtonEnum.Zero
BIGGEngine.MouseButtonEnum.Right   = BIGGEngine.MouseButtonEnum.One
BIGGEngine.MouseButtonEnum.Middle  = BIGGEngine.MouseButtonEnum.Two

BIGGEngine.ActionEnum = {}
BIGGEngine.ActionEnum.Release = 0
BIGGEngine.ActionEnum.Press   = 1
BIGGEngine.ActionEnum.Repeat  = 2

BIGGEngine.ModsEnum = {}
BIGGEngine.ModsEnum.Shift     = 0x0001
BIGGEngine.ModsEnum.Control   = 0x0002
BIGGEngine.ModsEnum.Alt       = 0x0004
BIGGEngine.ModsEnum.Super     = 0x0008
BIGGEngine.ModsEnum.CapsLock  = 0x0010
BIGGEngine.ModsEnum.NumLock   = 0x0020

return BIGGEngine