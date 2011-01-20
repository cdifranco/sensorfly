function [succ sigRoute] = navigate_basic(packet_id, node_id, d_input_stream, d_output_stream, port, destArea, base_number, transHistory, center_sig, count_to_id, matrix, area_cluster_relation)
%% Init
step_threshold = 50;
succ = 1;
totalCnt = 0;
%directionNumber = size(transHistory, 2);
fprintf('read current signature');
[currentSig packet_id] = get_sig_from_port(packet_id, port, base_number);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:base_number) = currentSig;
currentCluster = get_cluster_sig(center_sig, currentSig)
%destCluster = get_cluster_area(reading, destArea, size(center_sig,1), count_to_id)
destCluster = area_cluster_relation{destArea}(1:20)
%% Main while loop
while 1
    %%
    if  ~isempty(find(ismember(destCluster, currentCluster), 1))
        fprintf('u r in the dest area %d\n', destArea);
        socket_send(d_output_stream, 's');
        break;
    else
        %%
        current_dir = get_dir_from_port(packet_id, node_id, serial_port);
        [path direction_order] = guide(currentCluster, destCluster(1), transHistory, count_to_id, matrix);
        suggest_dir = uint32(direction_order(1));
        %% Current command
        if abs(current_dir - suggest_dir) == 2
            fprintf('go back');
            socket_send(d_output_stream, 'b');
        elseif (current_dir - suggest_dir) == 1 || (current_dir - suggest_dir) == -3
            fprintf('turn left');
            socket_send(d_output_stream, 'l');
        elseif (current_dir - suggest_dir) == -1 || (current_dir - suggest_dir) == 3
            fprintf('turn right');
            socket_send(d_output_stream, 'r');
        else
            fprintf('keep up');
            socket_send(d_output_stream, 'u');
        end
        %% Original command
        %{
        fprintf('at %d go: ', currentCluster);
        if isempty(path)
            fprintf(' random pick--> %d\n',uint32(unidrnd(directionNumber)));  
        else
            fprintf(' pick based on map--> %d\n',);
        end
        %}
        %%
        currentSig = get_sig_from_port(packet_id, port, base_number);
        totalCnt = totalCnt + 1;
        if totalCnt > step_threshold
            socket_send('e',125,10);
            succ = 0;
            break;
        end
        sigRoute(totalCnt, 1:base_number) = currentSig;
        currentCluster = get_cluster_sig(center_sig, currentSig);
    end
    %% Continue?
    %cont = input('continue?(yes:1/no:0)');    
    cont = socket_receive(d_input_stream, 1);
    if cont == 0
        break;
    end
end