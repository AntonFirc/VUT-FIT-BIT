package BlockScheme;

import java.io.Serializable;
import java.util.List;

import BlockScheme.Blocks.Block_Visual;

/**
 * Backendová třída pro blok.
 * @author xfolty15
 */
public class Block implements Serializable {
    public int ID;
    /**
     * Příchozí porty
     */
    public List<BlockPort> incomingPorts;

    /**
     * Odchozí porty
     */
    public List<BlockPort> outcomingPorts;

    /**
     * X pozice bloku
     */
    public double Xpos;

    /**
     * Y pozice bloku
     */
    public double Ypos;

    /**
     * Propojovací třída frontendu-backendu
     */
    public Block_Visual visual; //visual representation of block

    /**
     * Typ bloku
     */
    public int block_type = -1;

    public Block(List<BlockPort> incomingPorts, List<BlockPort> outcomingPorts) {
        this.incomingPorts = incomingPorts;
        this.outcomingPorts = outcomingPorts;
    }

    public void execute() {
    }

}
