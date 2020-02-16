package BlockScheme;

import java.io.Serializable;
import java.util.HashMap;

/**
 * Backendová třída pro port bloku.
 * @author xfolty15
 */
public class BlockPort implements Serializable {
    public HashMap<String, Double> type;
    public int con_block_id;

    public BlockPort(HashMap<String, Double> type) {
        this.type = type;
        this.con_block_id = -1;
    }
}
