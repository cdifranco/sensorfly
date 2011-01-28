% used in testing, generate next step in guidance
function next_step = next_step(current_point, suggest_dir, signatures)
step_len = (4+ unidrnd(5)); %step length is from 0.4m to 0.9m
angle = 2*pi*suggest_dir/4 + (rand*2-1)*pi/4;
tx = current_point(1)/10;
ty = current_point(2)/10;
isvalid_point = 0;
while ~isvalid_point
    new_x = (round(tx + step_len * cos(angle)))*10;
    new_y = (round(ty + step_len * sin(angle)))*10;
    if isempty(find(signatures(:,1)==new_x & signatures(:,2) == new_y, 1))
        continue;
    else
        isvalid_point = 1;
    end
end
next_step = [new_x, new_y];