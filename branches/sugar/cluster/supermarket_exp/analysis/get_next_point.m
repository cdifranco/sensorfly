% used for generate data set, used in generate cluster
function [new_x new_y compass_reading] = get_next_point(current_point, signatures)
tx = current_point(1);
ty = current_point(2);
reference_compass_reading = signatures(signatures(:,1) == current_point(1) & signatures(:,2) == current_point(2), 4);
isvalid_point = 0;
while ~isvalid_point
    step_len = 4 + unidrnd(3); %step length is from 0.6m to 0.9m
    angle = (rand*2-1)*pi;
    new_x = round(tx + step_len * cos(angle));
    new_y = round(ty + step_len * sin(angle));
    if isempty(find(signatures(:,1)==new_x & signatures(:,2) == new_y, 1))
        %fprintf('failed to generate: %d, %d\n',new_x,new_y);
        continue;
    else
        isvalid_point = 1;
    end
end
compass_reading = reference_compass_reading + (angle*180/pi-90)*10;
compass_reading =  round(mod(compass_reading, 3600));
