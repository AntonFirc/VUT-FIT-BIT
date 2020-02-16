package BlockScheme.Blocks;

import BlockScheme.Block;
import BlockScheme.BlockPort;

import java.util.List;

/**
 * Třída pro odčítání
 * @author xfolty15
 */
public class Block_Substract extends Block implements IBlock {
    public Block_Substract(List<BlockPort> incomingPorts, List<BlockPort> outcomingPorts) {
        super(incomingPorts, outcomingPorts);
    }

    @Override
    public void execute() {
        double first = incomingPorts.get(0).type.get("value");
        double second = incomingPorts.get(1).type.get("value");
        double result = first - second;
        outcomingPorts.get(0).type.put("value", result);
    }
}
