% used in testing, generate next step in guidance
function next_step = next_step(current_point, suggest_dir, signatures)
angle = 2*pi*(suggest_dir-1)/4 + (rand*2-1)*pi/4;
tx = current_point(1)/10;
ty = current_point(2)/10;
isvalid_point = 0;
invalid_count = 0;
while ~isvalid_point
    step_len = (5+ unidrnd(4)); %step length is from 0.5m to 0.9m
    new_x = (round(tx + step_len * cos(angle)))*10;
    new_y = (round(ty + step_len * sin(angle)))*10; %make sure it can be devided by 10 as each readings does
    if isempty(find(signatures(:,1)==new_x & signatures(:,2) == new_y, 1))
        invalid_count = invalid_count + 1;
        if invalid_count > 5
            angle = (rand*2-1)*pi;
        end
        continue;
    else
        isvalid_point = 1;
    end
end
next_step = [new_x, new_y];