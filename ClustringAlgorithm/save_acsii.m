function [] = save_acsii( file_name,value )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
fID = fopen(file_name, 'w');
for i = 1 : length(value)
    fprintf(fID,'%d\r\n',value(i));
end
fclose(fID);
end

