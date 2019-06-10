
Usage:
        make {(empty)|out|avm_exec|clean}:
                - (empty)      : clean up, then build out and avm_exec
                - out          : alpha language parser compilation recipe
                - avm_exec     : alpha language virtual machine executable compilation recipe
                - clean        : clean every executable and object
        
        out {file_path}
                Compiles and returns a binary file at given location with .abc extension.

        avm_exec {file_path}
                Runs the given file