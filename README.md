# DFS through parallel BFS in Directed Acyclic Graphs
This project was developed for the System and Device Programming course teached at Politecnico di Torino by Prof. Cabodi and Prof. Quer.

Some implementations are based on the ideas described in [Parallel Depth-First Search for Directed Acyclic Graphs](https://research.nvidia.com/sites/default/files/publications/nvr-2017-001.pdf).

Document containing project description and requirements is available [here](./q1.pdf).

The project report is available [here](https://docs.google.com/document/d/e/2PACX-1vT-IwaKr6j-OXrfqhCtB30pbp93gB3ZrZ6MBG6ERI9mJNJhOUh2-kGzFX7ajf8l1avRpMEoMDrARbeW/pub).

## Repository structure
This repository contains the C++ implementations of some parallel DFS algorithms, each branch contains a different algorithm, the branches with working code are:
* PathBasedThreadWorkers
* parallelSolution
* pipelineSolution
* list_based_solution

Other branches are home to experiments and abandoned code.

## Path-based DFS
This branch contains code to perform a parallel DFS using the Path-based approach, it also contains a sequential DFS implementation to compare results and performance of other solutions.

## Use

```bash
cd source
cmake .
make
./PathBasedThreadWorkers ../graph_examples/test.gra sequential
./PathBasedThreadWorkers ../graph_examples/test.gra parallel
```
