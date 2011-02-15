import java.net.ServerSocket
import java.io.*

port = 8964;
number_of_retries = 100; 
retry = 0;
message = ['r','f','b','l'];
connection  = [];
succ = 0;
count = 1;
%%
try
    % wait for 1 second for client to connect server socket
    server_socket = ServerSocket(port);
    server_socket.setSoTimeout(10000);
catch ME
    error('socket fail to open');
end
%%
while 1
    retry = retry + 1;
    try
        fprintf('start %d succ\n', succ);
        if ((number_of_retries > 0) && (retry > number_of_retries))
            fprintf(1, 'too many retries\n');
            break;
        end

        fprintf('try %d waiting for client to connect to this host on port : %d\n', retry, port);
    
        fprintf('wait for connection, get success connection %d once\n', succ);
        connection  = server_socket.accept;

        succ = succ + 1;
        fprintf('Connected\n');
        break;
    catch ME
        fprintf('Connection fail\n');
        pause(1);
    end
end
%%
while 1
    count = mod(count+1,4);
    try
        %%
        fprintf('set IO stream\n');
        output_stream   = connection.getOutputStream;
        input_stream = connection.getInputStream;
        d_output_stream = DataOutputStream(output_stream);
        d_input_stream = DataInputStream(input_stream);
        %%
        fprintf('start to receive\n');
        receive_msg = socket_receive(d_input_stream, 1)
        fprintf('received %d\n',char(receive_msg));
        if receive_msg == 0
            break;
        elseif ismember(2:27,receive_msg)
            fprintf('get area id\n');
        elseif receive_msg == 1
            fprintf('get continue\n');
        else
            fprintf(2, 'receive aaa\n');
        end
        %%
        % output the data over the DataOutputStream
        % Convert to stream of bytes
        
        fprintf('writing %d bytes\n', 1);
        socket_send(d_output_stream, message(count+1));
    catch ME
        fprintf('IO fail\n');
        %% close socket
        if ~isempty(server_socket)
            server_socket.close
        end

        if ~isempty(connection)
           connection.close
        end
        try
        server_socket = ServerSocket(port);
        server_socket.setSoTimeout(10000);
        connection  = server_socket.accept;
        catch ME
            fprintf('Connection fail\n');
            pause(1);
        end
        pause(1);
    end
end
fprintf('end of the process, ready to close socket\n');
%% Clean up
if ~isempty(server_socket)
    server_socket.close
end

if ~isempty(connection)
   connection.close
end
