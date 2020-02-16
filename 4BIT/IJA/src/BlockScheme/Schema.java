package BlockScheme;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Backendová třída pro blokové schéma.
 * @author xfolty15
 */
public class Schema implements Serializable {
    public String name;

    List<Block> blocks = new ArrayList<>();


    public Schema(String name) {

        /**
        List<BlockPort> incomingPorts = new ArrayList<>();
        List<BlockPort> outcomingPorts = new ArrayList<>();

        HashMap<String, Double> typeIncoming1 = new HashMap<>();
        typeIncoming1.put("value", 2.0);

        HashMap<String, Double> typeOutcoming1 = new HashMap<>();
        typeOutcoming1.put("value", null);

        BlockPort incoming1 = new BlockPort(typeIncoming1);
        BlockPort incoming2 = new BlockPort(typeIncoming1);
        BlockPort outcoming1 = new BlockPort(typeOutcoming1);

        incomingPorts.add(incoming1);
        incomingPorts.add(incoming2);
        outcomingPorts.add(outcoming1);

        Block block = new Block(incomingPorts, outcomingPorts);
        **/
    }
}
