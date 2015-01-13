%%-------------------------------------------------------------------
%% FUNCTION evaluation
%  This function is used to calculate the NMI and ACC of the community detection 
%  algorithm
%
%% INPUT
%   outputfile: filename of the output from the community detection algorithm
%   gdfile: filename of the ground truth
%   k: numbers of communities
%
%   PLEASE make sure that the length of the output file and the ground
%   truth file are matched.
%
%% OUTPUT
%   NMI: Normalized Mutual Information - double
%   ACC: Accuracy - double
%
%   These two indicators are used to measure the community detection
%   algorithms w.r.t. the ground truth. 
%   The higher, the better. All of them could be up to 1.
%
%% EXAMPLE
%  [NMI,ACC] = evaluation('.\output.txt', '.\polbooks_gd.txt', 6);
%
%% OTHER INFORMATION
%  Author: Aiqing Huang(Cassie)
%  Latest Modified: Dec.2013
%  
%  For any problem, please contact with Cassie Huang via cassie.a.q.huang@gmail.com 
%%-------------------------------------------------------------------
%% Code starts here
function [NMI,ACC] = evaluation(outputfile,gdfile,k)
    y_list = load(outputfile);
    gd_list = load(gdfile);

    [n,~]=size(y_list);

    %Get Communities
    y = {};
    gd = {};
    for i=1:1:k
        community = [];
        y{k,1} = community;
        gd{k,1} = community;
    end
    for i=1:1:n
        community = y{y_list(i,1),1};
        l = length(community);
        community(1,l+1)=i;
        y{y_list(i,1),1} = community;
    end
    for i=1:1:n
        community = gd{gd_list(i,1),1};
        l = length(community);
        community(1,l+1)=i;
        gd{gd_list(i,1),1} = community;
    end

    p_y = [];
    p_gd = [];

    %GetOverlapped
    for i=1:1:k
        for j=1:1:k
            temp = intersect(y{i,1},gd{j,1});
            [~,m]=size(temp);
            overlapped(i,j) = m;
        end
        p_y(1,i) = length(y{i,1});
        p_gd(1,i) = length(gd{i,1});
    end

    %calculate NMI
    NMI_overlapped = overlapped./n;
    p_y = p_y./n;
    p_gd = p_gd./n;
    paiewiseMI = NMI_overlapped.*log(NMI_overlapped./((p_y'*ones(1,k)).*(ones(k,1)*p_gd)));
    paiewiseMI(isnan(paiewiseMI)) = 0;
    MI = sum(sum(paiewiseMI));
    paiewise_H_y = p_y.*log(p_y);
    paiewise_H_y(isnan(paiewise_H_y)) = 0;
    H_y = sum(sum(paiewise_H_y));
    paiewise_H_gd = p_gd.*log(p_gd);
    paiewise_H_gd(isnan(paiewise_H_gd)) = 0;
    H_gd = sum(sum(paiewise_H_gd));
    NMI = -2*MI/(H_y+H_gd);



    %calculate ACC
    acc_num = 0;
    for i=1:1:k
        [~,max_row] = max(max(overlapped')');
        [~,max_col] = max(max(overlapped)');
        acc_num = acc_num + overlapped(max_row,max_col);
        overlapped(max_row,:) = [];
        overlapped(:,max_col) = [];   
    end
    ACC = acc_num/n;

end


