% used for generate data set, used in generate cluster
function [new_x new_y compass_reading] = get_next_point(current_point, signatures)
old_x = current_point(1);
old_y = current_point(2);
reference_compass_reading = signatures(signatures(:,1) == current_point(1) & signatures(:,2) == current_point(2), 4);
isvalid_point = 0;
while ~isvalid_point
    step_len = 2 + unidrnd(1); %step length is from 0.6m to 0.9m
    angle = (rand*2-1)*pi;
    new_x = round(old_x + step_len * cos(angle));
    new_y = round(old_y + step_len * sin(angle));
    % if the new location does not have reading in database, re-direct; 
    % if detect obstacle; e.g. cross rack, re-direct
    % fprintf('(%d,%d)-->(%d,%d) is blocked:%d\n',old_x,old_y,new_x,new_y,is_blocked(old_x, old_y, new_x, new_y));
    if isempty(find(signatures(:,1)==new_x & signatures(:,2) == new_y, 1)) 
        %fprintf('invalid sig: %d,%d\n', new_x, new_y);
        continue;
    elseif is_blocked(old_x, old_y, new_x, new_y)
        %fprintf('blocked\n');
        continue;
    else
        isvalid_point = 1;
        fprintf('new: %d, %d \n', new_x, new_y);
    end
end
compass_reading = reference_compass_reading + (angle*180/pi-90)*10;
compass_reading =  round(mod(compass_reading, 3600));
