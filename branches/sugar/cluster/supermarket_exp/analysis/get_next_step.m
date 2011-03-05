% used for generate next step in testing
function new_coord = get_next_step(direction, current_point, signatures)
tx = current_point(1);
ty = current_point(2);
angle = angle_convert(direction);
isvalid_point = 0;
while ~isvalid_point
    step_len = 1 + unidrnd(3); % step length is from (1~2)*2  
    new_x = tx + round(step_len * cos(angle*pi/180));
    new_y = ty + round(step_len * sin(angle*pi/180));
    new_coord = [new_x new_y];
    if isempty(find(signatures(:,1)==new_x & signatures(:,2) == new_y, 1))
        fprintf('failed to generate: %d, %d\n',new_x,new_y);
        continue;
    else
        isvalid_point = 1;
    end
end