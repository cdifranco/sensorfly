function blocked = is_blocked(old_x, old_y, new_x, new_y)
blocked = 0;
load 'environment_info.mat';
rack_edge_number = size(racks, 1);
for i = 1 : rack_edge_number
   % check every rack, see if the data showing cross rack movement
   if (old_x - racks(i, 1))*(new_x - racks(i, 1)) < 0 || (old_x - racks(i, 2))*(new_x - racks(i, 2)) < 0 ...
      || (old_y - racks(i, 3))*(old_y - racks(i, 4)) < 0 || (new_y - racks(i, 3))*(new_y - racks(i, 4)) < 0  
        % cross the edge of the rack
        blocked = 1;
   end
end