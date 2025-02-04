# Αποτελέσματα αλγορίθμων με κανονικό δείγμα αλλά και με το μειωμένο δείγμα

## LSH Algorithm with k=10 and L=11

| Query | tLSH (sec) | tLSH Reduced (sec) | DistLSH | DistLSH Reduced | Approximation Factor (AF) |
|-------|------------|--------------------|-------------|---------------------|------------------------------------------:|
| 1     | 0.075153   | 0.001289           | 1644        | 2330.13             | 1.41735                                  |
| 2     | 0.007755   | 0.000442           | 2388        | 2858.03             | 1.19683                                  |
| 3     | 0.004784   | 0.000388           | 660         | 2548.38             | 3.86118                                  |
| 4     | 0.005712   | 0.000424           | 1666        | 2706.02             | 1.62426                                  |
| 5     | 0.005778   | 0.00039            | 1712        | 2337.65             | 1.36545                                  |
| 6     | 0.00489    | 0.000431           | 621         | 2320.86             | 3.7373                                   |
| 7     | 0.00561    | 0.000409           | 1637        | 2682.1              | 1.63842                                  |
| 8     | 0.004828   | 0.000427           | 2012        | 2344.02             | 1.16502                                  |
| 9     | 0.005213   | 0.000446           | 2228        | 2710.31             | 1.21648                                  |
| 10    | 0.005909   | 0.00054            | 1678        | 3051.22             | 1.81837                                  |
|       |            |                    |             | **Average AF**      | **1.090407**                             |



## Hypercube Algorithm with k=10, M=100, and Probes=5

| Query | tCUBE (sec) | tCUBE Reduced (sec) | DistCUBE | DistCUBE Reduced | Approximation Factor (AF) |
|-------|-------------|---------------------|----------|------------------|---------------------------:|
| 1     | 0.004762    | 0.000299            | 1178     | 2419.52          |                    2.05392 |
| 2     | 0.004656    | 0.000296            | 1162     | 2564.15          |                    2.20667 |
| 3     | 0.004268    | 0.000283            | 706      | 1903.76          |                    2.69655 |
| 4     | 0.004938    | 0.000286            | 2139     | 2856.16          |                    1.33528 |
| 5     | 0.004543    | 0.000288            | 1599     | 2545.15          |                    1.59172 |
| 6     | 0.004198    | 0.000275            | 554      | 1601.44          |                    2.89068 |
| 7     | 0.004789    | 0.000303            | 1952     | 2204.31          |                    1.12926 |
| 8     | 0.004654    | 0.000304            | 1670     | 2898.52          |                    1.73564 |
| 9     | 0.004585    | 0.000295            | 2316     | 2577.48          |                    1.1129  |
| 10    | 0.004603    | 0.000294            | 1308     | 2557.91          |                    1.95559 |
|       |             |                     |          | **Average AF**   |                 **1.87082** |


## GNNS Algorithm with k=50 and E=100

| Query | tGNNS (sec) | tGNNS Reduced (sec) | DistGNNS | DistGNNS Reduced | Approximation Factor (AF) |
|-------|-------------|---------------------|----------|------------------|---------------------------:|
| 1     | 0.058814    | 0.013213            | 793      | 2405.17          |                    3.033   |
| 2     | 0.07956     | 0.004126            | 2060     | 3138.8           |                    1.52369 |
| 3     | 0.0577      | 0.004131            | 554      | 2808.67          |                    5.0698  |
| 4     | 0.081041    | 0.003287            | 1389     | 2663.25          |                    1.91739 |
| 5     | 0.066309    | 0.015948            | 1593     | 2310.61          |                    1.45048 |
| 6     | 0.057454    | 0.008167            | 567      | 2301.1           |                    4.05837 |
| 7     | 0.078902    | 0.003036            | 1717     | 2451.35          |                    1.42769 |
| 8     | 0.065119    | 0.003597            | 1505     | 2353.54          |                    1.56381 |
| 9     | 0.064334    | 0.013144            | 2042     | 2537.59          |                    1.2427  |
| 10    | 0.074816    | 0.008719            | 1424     | 2370.05          |                    1.66436 |
|       |             |                     |          | **Average AF**   |                 **2.29513** |


## MRNG Algorithm with I=20 and N=10

| Query | tMRNG (sec) | tMRNG Reduced (sec) | DistMRNG | DistMRNG Reduced | Approximation Factor (AF) |
|-------|-------------|---------------------|----------|------------------|---------------------------:|
| 1     | 0.000691    | 0.000293            | 1952     | 2849.37          |                    1.45972 |
| 2     | 0.000788    | 0.00026             | 2040     | 2690.07          |                    1.31866 |
| 3     | 0.000765    | 0.000342            | 2133     | 2580.91          |                    1.20999 |
| 4     | 0.000753    | 0.000248            | 1650     | 2963.23          |                    1.7959  |
| 5     | 0.000688    | 0.00027             | 1763     | 2290.3           |                    1.29909 |
| 6     | 0.000784    | 0.000289            | 2172     | 3623.15          |                    1.66812 |
| 7     | 0.000684    | 0.000293            | 1686     | 2944.19          |                    1.74626 |
| 8     | 0.000667    | 0.000247            | 2194     | 2736.89          |                    1.24744 |
| 9     | 0.000594    | 0.000242            | 1176     | 2764.75          |                    2.35098 |
| 10    | 0.000634    | 0.000242            | 1684     | 2631.54          |                    1.56267 |
|       |             |                     |          | **Average AF**   |                 **1.47779** |

