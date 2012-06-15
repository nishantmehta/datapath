/**
 * GetDPConfig.java
 *
 * @author Praveen Salitra (praveen@cise.ufl.edu)
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
package edu.ufl.cise.datapath;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import edu.ufl.cise.datapath.exception.DataPathError;

/**
 * Servlet implementation class GetDPConfig
 */
@WebServlet ("/GetDPConfig")
public class GetDPConfig extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public GetDPConfig ()
    {
        super ();
    }

    /**
     * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse
     *      response)
     */
    @Override
    protected void doGet (HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
        doPost (request, response);
    }

    /**
     * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
     *      response)
     */
    @Override
    protected void doPost (HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {
        PrintWriter out = response.getWriter ();
        ConnectDB connDB = new ConnectDB ();
        response.setContentType ("application/json");
        response.setHeader ("Access-Control-Allow-Origin", "*");
        connDB.connectToDB (DataPathConstants.DATAPATH_DB);
        PreparedStatement preStmt1 = null;
        ResultSet rs1 = null;
        PreparedStatement preStmt2 = null;
        ResultSet rs2 = null;
        PreparedStatement preStmt3 = null;
        ResultSet rs3 = null;
        PreparedStatement preStmt4 = null;
        ResultSet rs4 = null;
        PreparedStatement preStmt5 = null;
        ResultSet rs5 = null;
        PreparedStatement preStmt6 = null;
        ResultSet rs6 = null;
        try
        {
            JSONObject jsonObj = new JSONObject ();
            preStmt1 = connDB._conn.prepareStatement ("SELECT type FROM DataTypeManager_TypeInfo");
            rs1 = preStmt1.executeQuery ();
            JSONArray jsonTypeArr = new JSONArray ();
            while (rs1.next ())
            {
                jsonTypeArr.put (rs1.getString ("type"));
            }
            preStmt2 = connDB._conn.prepareStatement ("SELECT glaID, glaName, glaDesc, glaType FROM GLA_Info");
            rs2 = preStmt2.executeQuery ();
            JSONObject jsonGLAObj = new JSONObject ();
            while (rs2.next ())
            {
                jsonGLAObj.put (""+rs2.getInt ("glaID"), new JSONObject ().put ("gla_name", rs2.getString ("glaName"))
                                                                          .put ("gla_desc", rs2.getString ("glaDesc"))
                                                                          .put ("gla_type", rs2.getString ("glaType")));
            }
            
            preStmt3 = connDB._conn.prepareStatement ("SELECT pigID, pigName, pigDesc FROM Piggy_Info");
            rs3 = preStmt3.executeQuery ();
            JSONObject jsonPiggyObj = new JSONObject ();
            while (rs3.next ())
            {
                jsonPiggyObj.put (""+rs3.getInt ("pigID"), new JSONObject ().put ("pig_name", rs3.getString ("pigName"))
                                                                          .put ("pig_desc", rs3.getString ("pigDesc")));
            }
            
            preStmt4 = connDB._conn.prepareStatement ("SELECT relID, relName FROM Relations");
            rs4 = preStmt4.executeQuery ();
            JSONObject jsonRelObj = new JSONObject ();
            while (rs4.next ())
            {
                preStmt5 = connDB._conn.prepareStatement ("SELECT type, attribute from CatalogAttributes where relID = ?");
                preStmt5.setInt (1, rs4.getInt ("relID"));
                rs5 = preStmt5.executeQuery ();
                JSONArray jsonAtts = new JSONArray ();
                while(rs5.next ()){
                    jsonAtts.put (new JSONObject ().put ("att_type", rs5.getString ("type"))
                            .put ("att_name", rs5.getString ("attribute")));                    
                }
                jsonRelObj.put (rs4.getString ("relName"), jsonAtts);
            }
            
            preStmt6 = connDB._conn.prepareStatement ("SELECT rezID, rezName, rezTimestamp FROM Query_Results");
            rs6 = preStmt6.executeQuery ();
            JSONObject jsonRezObj = new JSONObject ();
            while (rs6.next ())
            {
                jsonRezObj.put (""+rs6.getInt ("rezID"), new JSONObject ().put ("rez_name", rs6.getString ("rezName"))
                                                                          .put ("rez_timestamp", rs6.getString ("rezTimestamp")));
            }
            
            jsonObj.put ("datapath_json", new JSONObject ().put ("types", jsonTypeArr)
                                                           .put ("glas", jsonGLAObj)
                                                           .put ("piggys", jsonPiggyObj)
                                                           .put ("rezs", jsonRezObj)
                                                           .put ("relations", jsonRelObj));
            
            
            
            response.setStatus (DataPathConstants.STATUS_OK);
            out.print (jsonObj.toString ());

        } catch (SQLException e)
        {
            connDB.rollbackDB ();
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "SQLite Error: "+ e.getMessage ()));
        } catch (JSONException e)
        {
            response.setStatus (DataPathConstants.INTERNAL_SERVER_ERROR);
            out.print (DataPathError.getErrorJson (DataPathConstants.INTERNAL_SERVER_ERROR, "JSON Error: "+ e.getMessage ()));
        } finally
        {
            connDB.closePR (preStmt1, rs1);
            connDB.closePR (preStmt2, rs2);
            connDB.closePR (preStmt3, rs3);
            connDB.closePR (preStmt4, rs4);            
            connDB.closePR (preStmt5, rs5);
            connDB.closePRC (preStmt6, rs6);
        }
    }
}
