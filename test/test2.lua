-- engine uses one single lua_State per script, so global objects shared between scriptComponents.

speed = 0.1
function BIGGEngine.WindowCreate()
    BIGGEngine.log("called WindowCreate!");
    pos = { this.transform.position[1], this.transform.position[2], this.transform.position[3] }
end

function BIGGEngine.MouseButton(button, action, mods)
    if action == BIGGEngine.ActionEnum.Press then


        this.transform.position = pos

        pos[1] = pos[1] + speed
        pos[2] = pos[2] + speed
        pos[3] = pos[3] + speed

        BIGGEngine.log("pos is %f %f %f", pos[1], pos[2], pos[3]);
    end
    return false
end
