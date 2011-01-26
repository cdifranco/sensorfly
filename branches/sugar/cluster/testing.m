%% Initialize socket
%{
import java.net.ServerSocket
import java.io.*
socket_port = 8964;
number_of_retries = 100; 
retry = 0;
message = '';
connection  = [];
succ_socket = 0;
try
    % wait for 1 second for client to connect server socket
    server_socket = ServerSocket(socket_port);
    server_socket.setSoTimeout(10000);
catch ME
    fprintf('socket fail to open');
end
%}
%% Initialization
clr = input('clear record?');
if clr == 1
    step = [];
    observed_step = [];
    error = [];
    success = [];
    start_round = 1;
else
    start_round = input('from which round:');
end
serial_port = serial(port, 'BaudRate', 38400, 'DataBits', 8, 'Timeout', 0.5);
%% Open socket
%{
while 1
    retry = retry + 1;
    try
        %%
        fprintf('start %d succ\n', succ_socket);
        if ((number_of_retries > 0) && (retry > number_of_retries))
            fprintf(1, 'Too many retries\n');
            break;
        end
        %%
        fprintf('try %d waiting for client to connect to this host on port : %d\n', retry, socket_port); 
        fprintf('wait for connection, get success connection %d once\n', succ_socket);
        connection  = server_socket.accept;
        %%
        succ_socket = succ_socket + 1;
        fprintf('connected\n');
        break;
    catch ME
        fprintf('connection fail\n');
    end
end
%% Set up socket IO
try
    output_stream   = connection.getOutputStream;
    input_stream = connection.getInputStream;
    d_output_stream = DataOutputStream(output_stream);
    d_input_stream = DataInputStream(input_stream);
catch ME
    fprintf('IO set up fail\n');
end
%}
%% Open serial port
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
%% Testing loop
for j = start_round:testing_round
    destArea = input('destiny area: ');
    %destArea = socket_receive(d_input_stream, 1);
     %% generate the matrix
    number_of_center = size(center_new,1);
    matrix = zeros(number_of_center, number_of_center);
    for l = 1:number_of_center
        for m = 1:number_of_center
            sumIJ = sum(trans_history(count_to_id(l),:,count_to_id(m)));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                sum_of_l_to_m = 0;
                for n = 1 : number_of_center
                    sum_of_l_to_m = sum_of_l_to_m + sum(trans_history(count_to_id(l),:,count_to_id(n)));
                end
                matrix(l, m) = sum(trans_history(count_to_id(l),:,count_to_id(m)))/sum_of_l_to_m;
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    %% call for navigate
    %[succ sigRoute] = navigate_basic(packet_id, node_id, d_input_stream, d_output_stream, serial_port, destArea, base_number, trans_history,center_sig, count_to_id, matrix, area_cluster_relation);% for socket
    %test
    [succ sigRoute] = navigate_basic(packet_id, node_id, serial_port, destArea, base_number, trans_history, center_sig, count_to_id, matrix, area_cluster_relation);
    %[succ sigRoute] = navigate_basic(packet_id, serial_port, reading, 
    %destArea, base_number, trans_history, center_sig, count_to_id, matrix, top);
    %% record metric
    fprintf('you have forwarded %d steps\n',size(sigRoute,1));
    s = input('success?(yes:1/no:0)');
    if s ~= 0
        success=  [success, 1];
        os = input('observed step: ');
        observed_step = [observed_step, os];
        e = input('error: ');
        error = [error, e];
        step = [step, size(sigRoute,1)];
    else
        success=  [success, 0];
        observed_step = [observed_step, inf];
        error = [error, inf];
        step = [step, inf];
    end
    save '1_25_morning_afterwards_movement_t_0p7_1.mat'
    %% continue
    cont = input('still continue? (yes:1/no:0)');
    if cont == 0
        break;
    end
end
step_rate = step(step(:)~=inf)/observed_step(observed_step(:)~=inf)
ave_error = sum(error(error(:)~=inf))/size(error(error(:)~=inf),2)
%% Close port
try    
    stopasync(serial_port);
    fclose(serial_port);
    delete(serial_port);
    clear serial_port;
catch ME
    % print out warning
    error('fail to close the serial port, check connection and name'); 
end
%% Close socket
%{
if ~isempty(server_socket)
    server_socket.close
end

if ~isempty(connection)
   connection.close
end
%}
%% Clear 
clear cont;
clear l;
clear m;
clear j;