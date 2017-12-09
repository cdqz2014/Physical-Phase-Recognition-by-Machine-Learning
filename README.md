
# Physical-Phase-Recognition-by-Machine-Learning
Final Project of the class 数据科学导论


# Project Outline

## High/Low-temperature Phase Recognition by Shallow NN

- Data (already in equilibrium), both for *training set* and *test set*, are all generated by **Monte Carlo simulation** (cf. *Notes of Computational Physics* by 丁泽军)
- Basic structure: 
    - Input layer ![](http://latex.codecogs.com/svg.latex?\mathbf{x}) is the configuration of ![](http://latex.codecogs.com/svg.latex?m\times\,m) spins (without flatten).
    - Layer one has merely three neurons, explained as **high-T phase**, **low-T phase**, and **critical phase**, and excited by **Relu**. 
    - Output layer has merely one neuron, excited by **Sigmoid**.

## Unsupervied Learning to distinguish high-T and Low-T phases
- (Not finished yet) Predict to use **k-means** algorithm

## Topological Phase Recongition by CNN
- Seems to have not enough time to finish

# Usage

All codes are checked on my **Archlinux** but **not on Windows**, so there may be some fatal errors that you need to debug youself...

## Ising Model Data
Data generation by Monte Carlo costs long time, so this module is written by ```C``` and **parallel scheduled** by ```python```.
If you wanna generate one configuration for test,
```shell
./Ising -n (size) -c (cycles to reach equilibrium) -t (temperature) -o (output name)
```
If you wanna generate large number of files,
```python
python data_generator.py
```
Note that data is automatically and randomly divided by two parts: training set, and test set.

## Visualization
Comput and draw all the magnetization of training set to visualize the phase transition structure, this is just for data check and not related to our main tasks. So I roughly wrote the code and it may need to be modified for your generated data
```python
python "Reading and Visualization.py"
```

## Phase Recognition of Ising Model
Use ```tensorflow (1.4.0 version)``` to learn to recognize phases (labeled manually) and ```Tensorboard``` to visualize the training results.

### Simple Fully Connected NN

```python
python "Phase Recongition-Fully Connnected.py"
```
### CNN

Undo